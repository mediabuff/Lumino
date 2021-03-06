﻿
#include "../Internal.h"
#include <hlsl2glsl.h>
#include "LuminoShader.h"

#include <Fluorite/AnalyzerContext.h>
#include <Fluorite/Diagnostics.h>
#include "../Source/Fluorite/Lexer/CppLexer.h"
#include "../Graphics/Device/DirectX9/DX9Module.h"

LN_NAMESPACE_BEGIN

struct HLSLPass
{
	std::string	name;
	std::string vertexShader;	// for Raw HLSL
	std::string pixelShader;	// for Raw HLSL
	std::string shadingModel;	// for Lumino HLSL
	std::string ligitingModel;	// for Lumino HLSL
	std::string surfaceShader;	// for Lumino HLSL
};

struct HLSLTechnique
{
	std::string	name;
	std::vector<HLSLPass>	passes;
};

class HLSLMetadataParser
{
public:
	std::vector<HLSLTechnique>	techniques;

	bool parse(fl::TranslationUnit* translationUnit);

	bool isLuminoShader() const { return m_isLuminoShader; }
	bool isLazyHLSLShader() const { return !isLuminoShader(); }

private:
	fl::Token* current() { return &m_tokens->getAt(m_current); }
	bool next();
	bool nextTo(char ch) { return nextTo(&ch, 1); }
	bool nextTo(const char* word, int len);
	bool isSpaceToken(const fl::Token& token) const;
	bool isEof() const;

	bool parseCompileUnit();
	bool parseTechnique(HLSLTechnique* tech);
	bool parsePass(HLSLPass* pass);
	bool parseRenderState(HLSLPass* pass);

	fl::TranslationUnit* m_translationUnit;
	List<fl::Token>* m_tokens;
	int m_current;
	bool m_isLuminoShader;
};

bool HLSLMetadataParser::parse(fl::TranslationUnit* translationUnit)
{
	m_current = 0;
	m_tokens = &translationUnit->m_tokens;
	m_isLuminoShader = false;

	parseCompileUnit();

	return true;
}

bool HLSLMetadataParser::next()
{
	do
	{
		m_current++;

	} while(isSpaceToken(m_tokens->getAt(m_current)));

	return !isEof();
}

bool HLSLMetadataParser::nextTo(const char* word, int len)
{
	do
	{
		m_current++;

	} while (
		isSpaceToken(m_tokens->getAt(m_current)) ||
		!m_tokens->getAt(m_current).equalString(word, len));

	return !isEof();
}

bool HLSLMetadataParser::isSpaceToken(const fl::Token& token) const
{
	return
		token.getTokenGroup() == fl::TokenGroup::SpaceSequence ||
		token.getTokenGroup() == fl::TokenGroup::NewLine ||
		token.getTokenGroup() == fl::TokenGroup::Comment;
}

bool HLSLMetadataParser::isEof() const
{
	return m_tokens->getAt(m_current).getTokenGroup() == fl::TokenGroup::Eof;
}

bool HLSLMetadataParser::parseCompileUnit()
{
	while (next())
	{
		if (current()->getTokenGroup() != fl::TokenGroup::Unknown)
		{
			if (current()->equalString("technique", 9))
			{
				HLSLTechnique tech;
				if (!parseTechnique(&tech)) return false;
				techniques.push_back(std::move(tech));
			}
		}
	}

	return true;
}

bool HLSLMetadataParser::parseTechnique(HLSLTechnique* tech)
{
	int begin = m_current;

	// 名前
	if (!next()) return false;
	if (current()->getTokenGroup() == fl::TokenGroup::Identifier)
	{
		tech->name = current()->getString();
	}
	else
	{
		LN_NOTIMPLEMENTED();
		return false;
	}

	if (!nextTo('{')) return false;

	bool closed = false;
	while (next())
	{
		if (current()->equalString("pass", 4))
		{
			HLSLPass pass;
			if (!parsePass(&pass)) return false;
			tech->passes.push_back(std::move(pass));
		}
		else if (current()->equalChar('}'))
		{
			closed = true;
			break;
		}
	}
	if (!closed) return false;

	// hlsl2glsl は technique ブロックを理解できないのですべて無効化しておく
	for (int i = begin; i <= m_current; i++)
	{
		m_tokens->getAt(i).setValid(false);
	}

	return true;
}

bool HLSLMetadataParser::parsePass(HLSLPass* pass)
{
	// 名前
	if (!next()) return false;
	if (current()->getTokenGroup() == fl::TokenGroup::Identifier)
	{
		pass->name = current()->getString();
	}
	else
	{
		LN_NOTIMPLEMENTED();
		return false;
	}

	if (!nextTo('{')) return false;

	bool closed = false;
	while (next())
	{
		if (current()->getTokenGroup() == fl::TokenGroup::Identifier)
		{
			if (!parseRenderState(pass)) return false;
		}
		else if (current()->equalChar('}'))
		{
			closed = true;
			break;
		}
	}
	if (!closed) return false;

	return true;
}

bool HLSLMetadataParser::parseRenderState(HLSLPass* pass)
{
	fl::Token* name = current();
	if (!nextTo('=')) return false;
	if (!next()) return false;
	fl::Token* value = current();
	
	if (name->equalString("VertexShader", 12))
	{
		next();	// skip "compile"
		next();	// skip "vs_x_x"
		pass->vertexShader = current()->getString();
	}
	else if (name->equalString("PixelShader", 11))
	{
		next();	// skip "compile"
		next();	// skip "ps_x_x"
		pass->pixelShader = current()->getString();
	}
	else if (name->equalString("ShadingModel", 12))
	{
		pass->shadingModel = current()->getString();
		m_isLuminoShader = true;
	}
	else if (name->equalString("LigitingModel", 13))
	{
		pass->ligitingModel = current()->getString();
		m_isLuminoShader = true;
	}
	else if (name->equalString("SurfaceShader", 13))
	{
		pass->surfaceShader = current()->getString();
		m_isLuminoShader = true;
	}

	if (!nextTo(';')) return false;

	return true;
}


//==============================================================================
// LuminoShaderContext
//==============================================================================
LuminoShaderContext::LuminoShaderContext()
{
}

void LuminoShaderContext::initialize()
{
	m_flContext = std::make_shared<fl::AnalyzerContext>();
	
	ByteBuffer buf;

	// Lumino.fxh.h
	{
		static const unsigned char data[] =
		{
#include "Resource/Lumino.fxh.h"
		};
		static const size_t size = LN_ARRAY_SIZE_OF(data);
		m_builtinShaderList.push_back({ "Lumino.fxh", std::string((const char*)data, size) });
	}

	// LuminoPBR.fxh.h
	{
		static const unsigned char data[] =
		{
#include "Resource/LuminoPBR.fxh.h"
		};
		static const size_t size = LN_ARRAY_SIZE_OF(data);
		m_builtinShaderList.push_back({ "LuminoPBR.fxh", std::string((const char*)data, size) });
	}

	// LuminoShadow.fxh.h
	{
		static const unsigned char data[] =
		{
#include "Resource/LuminoShadow.fxh.h"
		};
		static const size_t size = LN_ARRAY_SIZE_OF(data);
		m_builtinShaderList.push_back({ "LuminoShadow.fxh", std::string((const char*)data, size) });
	}
}

bool LuminoShaderContext::findBuiltinShaderCode(const char* pathBegin, const char* pathEnd, const char** codeBegin, const char** codeEnd)
{
	size_t len1 = pathEnd - pathBegin;
	for (auto& info : m_builtinShaderList)
	{
		if (StringTraits::compare(pathBegin, len1, info.first.c_str(), info.first.length(), std::max(len1, info.first.length()), CaseSensitivity::CaseInsensitive) == 0)
		{
			*codeBegin = info.second.c_str();
			*codeEnd = info.second.c_str() + info.second.length();
			return true;
		}
	}
	return false;
}


class LuminoID3DInclude
	: public ID3DXInclude
{
public:
	LuminoID3DInclude(LuminoShaderContext* context)
		: m_context(context)
	{
	}

	virtual HRESULT STDMETHODCALLTYPE Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
	{
		switch (IncludeType)
		{
			case D3DXINC_LOCAL:		// #include ""
			case D3DXINC_SYSTEM:	// #include <>
				// 区別しない
				break;
			default:
				return E_FAIL;
		}

		size_t len = strlen(pFileName);
		const char* codeBegin;
		const char* codeEnd;
		if (m_context->findBuiltinShaderCode(pFileName, pFileName + len, &codeBegin, &codeEnd))
		{
			*ppData = codeBegin;
			*pBytes = codeEnd - codeBegin;
			return S_OK;
		}
		else
		{
			// TODO: とりあえず組み込みヘッダだけ許可
			return E_FAIL;
		}

		return E_FAIL;
	}

	virtual HRESULT STDMETHODCALLTYPE Close(LPCVOID pData) override
	{
		return S_OK;
	}

private:
	LuminoShaderContext*	m_context;
};

//==============================================================================
// LuminoShaderIRGenerater
//==============================================================================
LuminoShaderIRGenerater::LuminoShaderIRGenerater()
{

}

void LuminoShaderIRGenerater::initialize(LuminoShaderContext* context)
{
	m_context = context;
	int r = Hlsl2Glsl_Initialize();
	LN_ENSURE(r != 0, "failed Hlsl2Glsl_Initialize.");
}

void LuminoShaderIRGenerater::finalize()
{
	Hlsl2Glsl_Shutdown();
}

static bool Hlsl2Glsl(const std::string& input, const std::string& entryPoint, EShLanguage codeType, std::string* output, std::string* log)
{
	ShHandle shParser = nullptr;

	try
	{
		static EAttribSemantic kAttribSemantic[] =
		{
			EAttrSemPosition,
			EAttrSemPosition1,
			EAttrSemPosition2,
			EAttrSemPosition3,
			EAttrSemVPos,
			EAttrSemVFace,
			EAttrSemNormal,
			EAttrSemNormal1,
			EAttrSemNormal2,
			EAttrSemNormal3,
			EAttrSemColor0,
			EAttrSemColor1,
			EAttrSemColor2,
			EAttrSemColor3,
			EAttrSemTex0,
			EAttrSemTex1,
			EAttrSemTex2,
			EAttrSemTex3,
			EAttrSemTex4,
			EAttrSemTex5,
			EAttrSemTex6,
			EAttrSemTex7,
			EAttrSemTex8,
			EAttrSemTex9,
			EAttrSemTangent,
			EAttrSemTangent1,
			EAttrSemTangent2,
			EAttrSemTangent3,
			EAttrSemBinormal,
			EAttrSemBinormal1,
			EAttrSemBinormal2,
			EAttrSemBinormal3,
			EAttrSemBlendWeight,
			EAttrSemBlendWeight1,
			EAttrSemBlendWeight2,
			EAttrSemBlendWeight3,
			EAttrSemBlendIndices,
			EAttrSemBlendIndices1,
			EAttrSemBlendIndices2,
			EAttrSemBlendIndices3,
			EAttrSemPSize,
			EAttrSemPSize1,
			EAttrSemPSize2,
			EAttrSemPSize3,
			EAttrSemDepth,
			//EAttrSemUnknown,
			//EAttrSemVertexID,
			//EAttrSemInstanceID,
			//EAttrSemPrimitiveID,
			//EAttrSemCoverage,
		};
		static const char* kAttribString[] =
		{	// TODO: このへん、使える attr かどうかチェックしたい
			"ln_Vertex", //EAttrSemPosition,
			"ln_Vertex1", //EAttrSemPosition1,
			"ln_Vertex2", //EAttrSemPosition2,
			"ln_Vertex3", //EAttrSemPosition3,
			"ln_VPos",	//EAttrSemVPos,
			"ln_VFace", //EAttrSemVFace,
			"ln_Normal",	//EAttrSemNormal,
			"ln_Normal1",	//EAttrSemNormal1,
			"ln_Normal2",	//EAttrSemNormal2,
			"ln_Normal3",	//EAttrSemNormal3,
			"ln_Color0",	//EAttrSemColor0,
			"ln_Color1",	//EAttrSemColor1,
			"ln_Color2",	//EAttrSemColor2,
			"ln_Color3",	//EAttrSemColor3,
			"ln_MultiTexCoord0",	//EAttrSemTex0,
			"ln_MultiTexCoord1",	//EAttrSemTex1,
			"ln_MultiTexCoord2",	//EAttrSemTex2,
			"ln_MultiTexCoord3",	//EAttrSemTex3,
			"ln_MultiTexCoord4",	//EAttrSemTex4,
			"ln_MultiTexCoord5",	//EAttrSemTex5,
			"ln_MultiTexCoord6",	//EAttrSemTex6,
			"ln_MultiTexCoord7",	//EAttrSemTex7,
			"ln_MultiTexCoord8",	//EAttrSemTex8,
			"ln_MultiTexCoord9",	//EAttrSemTex9,
			"ln_Tangent",	//EAttrSemTangent,
			"ln_Tangent1",	//EAttrSemTangent1,
			"ln_Tangent2",	//EAttrSemTangent2,
			"ln_Tangent3",	//EAttrSemTangent3,
			"ln_Binormal",	//EAttrSemBinormal,
			"ln_Binormal1",	//EAttrSemBinormal1,
			"ln_Binormal2",	//EAttrSemBinormal2,
			"ln_Binormal3",	//EAttrSemBinormal3,
			"ln_BlendWeight",	//EAttrSemBlendWeight,
			"ln_BlendWeight1",	//EAttrSemBlendWeight1,
			"ln_BlendWeight2",	//EAttrSemBlendWeight2,
			"ln_BlendWeight3",	//EAttrSemBlendWeight3,
			"ln_BlendIndices",	//EAttrSemBlendIndices,
			"ln_BlendIndices1",	//EAttrSemBlendIndices1,
			"ln_BlendIndices2",	//EAttrSemBlendIndices2,
			"ln_BlendIndices3",	//EAttrSemBlendIndices3,
			"ln_PSize",	//EAttrSemPSize,
			"ln_PSize1",	//EAttrSemPSize1,
			"ln_PSize2",	//EAttrSemPSize2,
			"ln_PSize3",	//EAttrSemPSize3,
			"ln_Depth",	//EAttrSemDepth,
						//"ln_Unknown",	//EAttrSemUnknown,
						//"ln_VertexId",	//EAttrSemVertexID,
						//"ln_InstanceId",	//EAttrSemInstanceID,
						//"ln_PrimitiveId",	//EAttrSemPrimitiveID,
						//"ln_Coverage",	//EAttrSemCoverage,
		};


		shParser = Hlsl2Glsl_ConstructCompiler(codeType);
		if (shParser == 0)
		{
			(*log) += "failed ConstructCompiler.";
			return false;
		}

		int setUserAttrOk = Hlsl2Glsl_SetUserAttributeNames(shParser, kAttribSemantic, kAttribString, LN_ARRAY_SIZE_OF(kAttribSemantic));
		if (setUserAttrOk == 0)
		{
			(*log) += "failed SetUserAttributeNames.";
			return false;
		}

		unsigned options = 0;
		//if (false)
		//	options |= ETranslateOpIntermediate;
		ETargetVersion version = ETargetGLSL_120;//ETargetGLSL_ES_300;//
		int parseOk = Hlsl2Glsl_Parse(shParser, input.c_str(), version, nullptr, options);
		(*log) += Hlsl2Glsl_GetInfoLog(shParser);
		if (parseOk == 0)
		{
			(*log) += "failed Parse.";
			return false;
		}

		int translateOk = Hlsl2Glsl_Translate(shParser, entryPoint.c_str(), version, options);
		if (translateOk == 0)
		{
			(*log) += "failed Translate.";
			return false;
		}

		*output = Hlsl2Glsl_GetShader(shParser);

		Hlsl2Glsl_DestructCompiler(shParser);
	}
	catch (...)
	{
		if (shParser != 0)
		{
			Hlsl2Glsl_DestructCompiler(shParser);
		}
		throw;
	}

	return true;
}

bool LuminoShaderIRGenerater::convert(const char* input, int len, std::string* outCode, std::string* log)
{
	if (LN_REQUIRE(input)) return false;

	fl::AnalyzerContext ctx;
	auto file = ctx.RegisterInputMemoryCode(std::string{}, input, len);
	ctx.LexFile(file);
	file->createTranslationUnit();
	fl::TranslationUnit* tu = file->getTranslationUnit();
	tu->expandTokensOneFile(file);
	List<fl::Token>* tokens = &tu->m_tokens;

	HLSLMetadataParser parser;
	parser.parse(tu);

	if (parser.isLuminoShader())
	{
		LN_NOTIMPLEMENTED();
		return false;
	}
	else if (parser.isLazyHLSLShader())
	{

	}
	else
	{
		LN_UNREACHABLE();
		return false;
	}

	LN_NOTIMPLEMENTED();
}

bool LuminoShaderIRGenerater::convertRawHLSL_To_IRHLSL(const char* input, int len, std::string* outCode, std::string* log)
{
	if (LN_REQUIRE(input)) return false;

	DX9Module::initialize();

	LuminoID3DInclude d3dInclude(m_context);
	ID3DXBuffer* pShaderText = NULL;
	ID3DXBuffer* pErrorMsgs = NULL;
	HRESULT hr = DX9Module::D3DXPreprocessShader(input, len, NULL, &d3dInclude, &pShaderText, &pErrorMsgs);
	if (pErrorMsgs)
	{
		(*log) += std::string((const char*)pErrorMsgs->GetBufferPointer(), pErrorMsgs->GetBufferSize());
		pErrorMsgs->Release();
	}
	if (FAILED(hr))
	{
		return false;
	}

	(*outCode) = std::string((const char*)pShaderText->GetBufferPointer(), pShaderText->GetBufferSize());

	return true;
}

bool LuminoShaderIRGenerater::convertRawHLSL_To_IRGLSL(const char* input, int len, std::vector<LuminoShaderIRTechnique>* outTechniques, std::string* log)
{
	std::string hlslCode;
	if (!convertRawHLSL_To_IRHLSL(input, len, &hlslCode, log))
	{
		LN_NOTIMPLEMENTED();
		return false;
	}

	fl::AnalyzerContext ctx;
	auto file = ctx.RegisterInputMemoryCode(std::string{}, hlslCode.c_str(), hlslCode.length());
	ctx.LexFile(file);
	file->createTranslationUnit();
	fl::TranslationUnit* tu = file->getTranslationUnit();
	tu->expandTokensOneFile(file);
	List<fl::Token>* tokens = &tu->m_tokens;

	HLSLMetadataParser parser;
	if (!parser.parse(tu))
	{
		LN_NOTIMPLEMENTED();
		return false;
	}

	std::string minHLSL = tu->getStringValidCode();
	FileSystem::writeAllBytes(_T("test.fx"), minHLSL.c_str(), minHLSL.length());
	for (auto& tech : parser.techniques)
	{
		LuminoShaderIRTechnique irTech;
		irTech.name = tech.name;

		for (auto& pass : tech.passes)
		{
			LuminoShaderIRPass irPass;
			irPass.name = pass.name;
			irPass.vertexShaderEntryPoint = pass.vertexShader;
			irPass.fragmentShaderEntryPoint = pass.pixelShader;

			if (!Hlsl2Glsl(minHLSL, pass.vertexShader, EShLangVertex, &irPass.vertexShaderCode, log))
			{
				LN_NOTIMPLEMENTED();
				return false;
			}
			if (!Hlsl2Glsl(minHLSL, pass.pixelShader, EShLangFragment, &irPass.fragmentShaderCode, log))
			{
				LN_NOTIMPLEMENTED();
				return false;
			}

			irTech.passes.emplace_back(irPass);

			//FileSystem::writeAllBytes(_T("test.frag"), glslCode.c_str(), glslCode.length());

			// TODO: float3 BRDF_Diffuse_Lambert(const in float3 diffuseColor)
			// のように、"in" キーワードがあると変換エラーになる。これは lex だけでも、警告はできそう。

			// TODO: コンパイル時、sampler キーワードはエラーとなる。sampler2D とか明示する。そんな警告。
		}

		outTechniques->emplace_back(irTech);
	}
}

void LuminoShaderIRGenerater::convertRawHLSL_To_IncludeResolvedHLSLCode(const std::string& code)
{
	//fl::InputFile f(std::string{}, code.c_str(), code.length());
	//fl::CppLexer lex;
	////fl::TokenList tokenList;
	////fl::DiagnosticsItemSet diag();
	//lex.Tokenize(&f);

	fl::AnalyzerContext ctx;
	auto file = ctx.RegisterInputMemoryCode(std::string{}, code.c_str(), code.length());
	ctx.LexFile(file);
	file->createTranslationUnit();
	fl::TranslationUnit* tu = file->getTranslationUnit();
	tu->expandTokensOneFile(file);
	List<fl::Token>* tokens = &tu->m_tokens;
	//auto tokens = file->GetTokenList();

	HLSLMetadataParser parser;
	parser.parse(tu);

	{
		for (int i = 0; i < tokens->getCount(); i++)
		{
			if (tokens->getAt(i).getTokenType() == fl::TT_HeaderName)
			{
				const fl::Token& t = tokens->getAt(i);
				fl::StringRef ts = t.strRef();
				if (ts[0] == '<')
				{
					const char* codeBegin;
					const char* codeEnd;
					if (m_context->findBuiltinShaderCode(ts.begin() + 1, ts.end() - 1, &codeBegin, &codeEnd))
					{
						// #include 開始の # を探す
						for (int i2 = i; i2 >= 0; i2--)
						{
							tokens->getAt(i2).setValid(false);
							if (tokens->getAt(i2).equalChar('#'))
							{
								tu->insertToken(i2, std::string(codeBegin, codeEnd));
								i++;
								break;
							}
						}
					}
					//printf(tokens->getAt(i).getString().c_str());
				}
			}
		}
	}



	auto minHLSL2 = tu->getStringValidCode();
	FileSystem::writeAllBytes(_T("test.fx"), minHLSL2.c_str(), minHLSL2.length());


	//HLSLMetadataParser parser;
	//parser.parse(tu);

	//auto minHLSL = tu->getStringValidCode();
	//FileSystem::writeAllBytes(_T("test.fx"), minHLSL.c_str(), minHLSL.length());

	//std::string glslCode, log;
	//Hlsl2Glsl(minHLSL, parser.techniques[0].passes[0].vertexShader, EShLangVertex, &glslCode, &log);
	//FileSystem::writeAllBytes(_T("test.vert"), glslCode.c_str(), glslCode.length());
	//Hlsl2Glsl(minHLSL, parser.techniques[0].passes[0].pixelShader, EShLangFragment, &glslCode, &log);
	//FileSystem::writeAllBytes(_T("test.frag"), glslCode.c_str(), glslCode.length());

	//printf("");
}

//std::string LuminoShaderIRGenerater::generateIRCode()
//{
//	return std::string();
//}

LuminoShader::LuminoShader()
{

}

LN_NAMESPACE_END
