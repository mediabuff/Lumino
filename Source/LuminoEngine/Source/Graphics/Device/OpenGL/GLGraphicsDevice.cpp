﻿/*
 *	EXT とか ARB とか
 *	https://shikihuiku.wordpress.com/2013/10/08/hardtounderstandoglext/
 */
#include "../../Internal.h"
#include <Lumino/Graphics/Utils.h>
#include <Lumino/Platform/PlatformWindow.h>
#include "GLSwapChain.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "GLGraphicsDevice.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN
namespace Driver
{

//==============================================================================
// GLGraphicsDevice
//==============================================================================

//------------------------------------------------------------------------------
GLGraphicsDevice::GLGraphicsDevice()
	: m_deviceState(DeviceState_Lost)
	, m_renderer(nullptr)
	, m_openGLMajorVersion(3)
	, m_openGLMinorVersion(1)
	, m_uniformTempBuffer()
	, m_uniformTempBufferWriter(&m_uniformTempBuffer)
{
}

//------------------------------------------------------------------------------
GLGraphicsDevice::~GLGraphicsDevice()
{
	LN_ASSERT(m_renderer == nullptr);	// dispose 済みであること
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::initialize(const ConfigData& configData)
{
	if (LN_REQUIRE(configData.mainWindow != nullptr)) return;

	m_mainWindow = configData.mainWindow;
	m_deviceState = DeviceState_Enabled;

	Logger::writeLine("GLGraphicsDevice::Initialize");
	Logger::writeLine("    Requested OpenGL version : %d.%d", configData.openGLMajorVersion, configData.openGLMinorVersion);

	// create main context
	m_mainContext = initializeMainContext(configData);

	// Threading: create new rendering context
	// Immediate: referemce main context
	if (configData.createSharedRenderingContext)
		m_mainRenderingContext = createContext(m_mainWindow);
	else
		m_mainRenderingContext = m_mainContext;

	// m_defaultSwapChain->create() でシェーダとか作るので先にアクティブにしておく
	makeCurrentContext(m_mainContext);

	// create MainWindow SwapChain
	Ref<GLContext> swapChainContext = m_mainContext;
	if (configData.createSharedRenderingContext)
		swapChainContext = createContext(m_mainWindow);
	m_defaultSwapChain = Ref<GLSwapChain>::makeRef();
	m_defaultSwapChain->initialize(this, swapChainContext, m_mainWindow);

	// create Renderer
	m_renderer = Ref<GLRenderer>::makeRef();
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::dispose()	// 仮想関数呼び出しが必要なのでデストラクタとは別に開放用関数を用意した
{
	ScopedAccessContext lock(this);
	GraphicsDeviceBase::dispose();

	m_renderer.safeRelease();
	m_defaultSwapChain.safeRelease();
	m_mainRenderingContext.safeRelease();
	m_mainContext.safeRelease();
}

//------------------------------------------------------------------------------
GLContext* GLGraphicsDevice::getMainContext() const
{
	return m_mainContext;
}

//------------------------------------------------------------------------------
GLContext* GLGraphicsDevice::getMainRenderingContext() const
{
	return m_mainRenderingContext;
}

//------------------------------------------------------------------------------
ISwapChain* GLGraphicsDevice::getDefaultSwapChain()
{
	return m_defaultSwapChain;
}

Ref<IShader> GLGraphicsDevice::createShader(const std::vector<LuminoShaderIRTechnique>& techniques, ShaderCompileResult* result)
{
	auto shader = Ref<GLShader>::makeRef();
	shader->initialize(this, techniques);
	result->Level = shader->getDiag()->level;
	result->Message = shader->getDiag()->message;
	return Ref<IShader>::staticCast(shader);
}

//------------------------------------------------------------------------------
ISwapChain* GLGraphicsDevice::createSwapChain(PlatformWindow* window)
{
	Ref<GLContext> context = createContext(window);
	auto ptr = Ref<GLSwapChain>::makeRef();
	ptr->initialize(this, context, window);
	return ptr.detachMove();
}

//------------------------------------------------------------------------------
Ref<IVertexDeclaration> GLGraphicsDevice::createVertexDeclarationImplement(const VertexElement* elements, int elementsCount)
{
	Ref<GLVertexDeclaration> obj(LN_NEW GLVertexDeclaration(), false);
	obj->initialize(elements, elementsCount);
	return Ref<IVertexDeclaration>::staticCast(obj);
}

//------------------------------------------------------------------------------
Ref<IVertexBuffer> GLGraphicsDevice::createVertexBufferImplement(size_t bufferSize, const void* data, ResourceUsage usage)
{
	Ref<GLVertexBuffer> obj(LN_NEW GLVertexBuffer(), false);
	obj->create(bufferSize, data, usage);
    return Ref<IVertexBuffer>::staticCast(obj);
}

//------------------------------------------------------------------------------
Ref<IIndexBuffer> GLGraphicsDevice::createIndexBufferImplement(int indexCount, const void* initialData, IndexBufferFormat format, ResourceUsage usage)
{
	Ref<GLIndexBuffer> obj(LN_NEW GLIndexBuffer(), false);
    obj->create(indexCount, initialData, format, usage);
    return Ref<IIndexBuffer>::staticCast(obj);
}

//------------------------------------------------------------------------------
Ref<ITexture> GLGraphicsDevice::createTextureImplement(const SizeI& size, bool mipmap, TextureFormat format, const void* initialData)
{
	Ref<GLTexture> obj(LN_NEW GLTexture(size, format, mipmap), false);
	if (initialData != nullptr) {
		obj->setSubData(PointI(0, 0), initialData, Utils::getTextureFormatByteCount(format) * size.width * size.height, size);
    }
    return Ref<ITexture>::staticCast(obj);
}

//------------------------------------------------------------------------------
Ref<ITexture> GLGraphicsDevice::createTexture3DImplement(int width, int height, int depth, uint32_t mipLevels, TextureFormat format, ResourceUsage usage, const void* initialData)
{
	LN_NOTIMPLEMENTED();
	return nullptr;
}

//------------------------------------------------------------------------------
Ref<ITexture> GLGraphicsDevice::ceateRenderTargetImplement(uint32_t width, uint32_t height, uint32_t mipLevels, TextureFormat format)
{
	Ref<GLRenderTargetTexture> obj(LN_NEW GLRenderTargetTexture(SizeI(width, height), format, mipLevels), false);
    return Ref<ITexture>::staticCast(obj);
}

//------------------------------------------------------------------------------
Ref<ITexture> GLGraphicsDevice::createDepthBufferImplement(uint32_t width, uint32_t height, TextureFormat format)
{
	Ref<GLDepthBuffer> obj(LN_NEW GLDepthBuffer(SizeI(width, height), format), false);
    return Ref<ITexture>::staticCast(obj);
}

//------------------------------------------------------------------------------
Ref<IShader> GLGraphicsDevice::createShaderImplement(const void* textData, size_t size, ShaderCompileResult* result)
{
	GLShader* shader = LN_NEW GLShader();
	shader->initialize(this, textData, size);
	result->Level = shader->getDiag()->level;
	result->Message = shader->getDiag()->message;

	//GLShader* shader = NULL;
	//result->Level = GLSLUtils::Build(this, textData, size, &shader, &result->Message);



	//if (shader != NULL) {
	//	addDeviceResource(shader);
	//}
	/*
		シェーダを作った直後、Shader の apply() → drawPrimitive() → present() すると、
		glFlush() とか wglMakeCurrent() とかでビジー状態になり、「ディスプレイドライバが応答しません」とか右下からエラー出た。
		リソースをメインスレッドで作って、描画スレッドで ドライバに描画情報送るときに落ちたと思われるが、ホントの原因は不明。
		SwapChain を 2 つ作ったりすると何故か正常に動作した。

		http://stackoverflow.com/questions/21613842/wglmakecurrent-in-second-thread-fails
		によると、グラボによってはバグを回避するために wglMakeCurrent(NULL, NULL) を仕込む必要があるとのこと。
		とりあえずそのようにしてみたところ、うまく動作するようになった。
		問題のグラボは AMD Radeon(TM) HD8490。

		GeForce GTX 560M では正常に動作した。
	*/
	//makeCurrentContext(NULL);
	//makeCurrentContext(getMainContext());

	Ref<IShader> obj(shader, false);
	return obj;
}

//------------------------------------------------------------------------------
Ref<ISwapChain> GLGraphicsDevice::createSwapChainImplement(PlatformWindow* window)
{
	LN_NOTIMPLEMENTED();
	return nullptr;
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::resetDevice()
{
	// 先に onLostDevice() を呼ぶこと
	LN_REQUIRE(m_deviceState == DeviceState_Pausing);
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::onLostDevice()
{
	m_deviceState = DeviceState_Pausing;
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::onResetDevice()
{
	m_deviceState = DeviceState_Enabled;
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::flushResource()
{
	/*
		- AMD Radeon(TM) HD8490
			glTexSubImage2D() などで設定されたデータは、その時点ではまだ VRAM に転送されない。
			MakeCurrent(NULL) されたり、glFlush() で始めて転送される。
			
			Threading モードの場合、メインスレッドは描画はしないがどこかで glFlush() しておかないと、
			別スレッドで描画しようとしたときに実はまだ VRAM にデータが無い、ということになってしまう。
			(DirectX はそのへん面倒見てくれたが・・・)
	*/

	glFlush();
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::parseGLVersion(int* glMajor, int* glMinor, int* glslMajor, int* glslMinor)
{
	// GL_VERSION の文字列フォーマットは決まっている。
	// https://www.opengl.org/wiki/GLAPI/glGetString
	// 現在は 1桁バージョンしかないので、Haskell の GLHUI はこんな単純なとり方をしていたりする。
	// http://hackage.haskell.org/package/GLHUI-1.1.0/src/c/HsGLWindow.c
	const char* version = (const char*)glGetString(GL_VERSION);
	*glMajor = version[0] - '0';
	*glMinor = version[2] - '0';
	if ((*glMajor) >= 2) {
		version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		*glslMajor = version[0] - '0';
		*glslMinor = version[2] - '0';
	}
	else {
		*glslMajor = 0;
		*glslMinor = 0;
	}
}

//------------------------------------------------------------------------------
bool GLGraphicsDevice::containsExtensionString(const char* extensionString, const char* str)
{
	const char* readPos = extensionString;
	const char* extensionStringEnd = extensionString + strlen(extensionString);
	while (true)
	{
		const char* found = strstr(readPos, str);
		if (found == NULL) {
			return false;
		}

		// 見つかったところは、ホントにトークンの先頭を指しているか
		if (found == readPos || *(found - 1) == ' ')
		{
			const char* endPos = found + strlen(str);
			if (endPos <= extensionStringEnd)
			{
				// 見つかった範囲の終端は次のトークンとの区切りのスペース文字か、\0
				if (*endPos == ' ' || *endPos == '\0') {
					break;	// 見つかった
				}
			}
		}
	}

	// break で抜けてきて、true を返す
	return true;
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::selectGLVersion(int requestMajor, int requestMinor)
{
	if (requestMajor == 0)
	{
		// 要求されていない場合は環境のデフォルトバージョンでコンテキストを作成する
		m_openGLMajorVersion = 0;
		m_openGLMinorVersion = 0;
	}
	else
	{
		// OpenGL バージョンの選択 (最大でも GL_VERSION にする)
		int glMajor, glMinor, glslMajor, glslMinor;
		parseGLVersion(&glMajor, &glMinor, &glslMajor, &glslMinor);
		if (requestMajor > glMajor) {
			m_openGLMajorVersion = glMajor;
			m_openGLMinorVersion = glMinor;
		}
		else if (requestMinor > glMinor) {
			m_openGLMajorVersion = requestMajor;
			m_openGLMinorVersion = glMinor;
		}
		else {
			m_openGLMajorVersion = requestMajor;
			m_openGLMinorVersion = requestMinor;
		}
	}

	Logger::writeLine("Active OpenGL version : %d.%d", m_openGLMajorVersion, m_openGLMinorVersion);
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::attachRenderingThread()
{
	makeCurrentContext(getMainRenderingContext());
	GraphicsDeviceBase::attachRenderingThread();
	m_renderer->activate();
}

//------------------------------------------------------------------------------
void GLGraphicsDevice::detachRenderingThread()
{
	m_renderer->Deactivate();
	makeCurrentContext(nullptr);
	GraphicsDeviceBase::detachRenderingThread();
}

} // namespace Driver
LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
