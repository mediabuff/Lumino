﻿/*
	FreeType はスレッドセーフではない。2.6 では FaceNew 等部分的に排他がかかるらしいが、
	公式では FT_Library をスレッドごとに作るのが一番簡単と言っている。
	でもそれでは全く同じ管理クラスが2つできることになり、使用メモリも増える。
*/
#include "../Internal.h"
#include <math.h>
#include <Lumino/Base/Hash.h>
#include <Lumino/Graphics/BitmapPainter.h>
#include <Lumino/Graphics/Utils.h>
#include <Lumino/Graphics/Texture.h>
#include "../RenderingCommand.h"
#include "../GraphicsManager.h"
#include "../Device/GraphicsDriverInterface.h"
#include "FontGlyphTextureCache.h"
#include "FontOutlineTessellator.h"

LN_NAMESPACE_BEGIN
namespace detail {

//==============================================================================
// FontGlyphTextureCache
//==============================================================================

//------------------------------------------------------------------------------
FontGlyphTextureCache::FontGlyphTextureCache()
	: m_manager(nullptr)
	, m_maxCacheGlyphs(0)
	, m_glyphWidthCount(0)
	, m_curPrimUsedCount(0)
	, m_fillGlyphsTexture(nullptr)
{
}

//------------------------------------------------------------------------------
FontGlyphTextureCache::~FontGlyphTextureCache()
{
}

//------------------------------------------------------------------------------
void FontGlyphTextureCache::initialize(GraphicsManager* manager, RawFont* font)
{
	m_manager = manager;
	m_font = font;
	m_glyphMaxBitmapSize = m_font->getGlyphMaxSize();
	m_layoutEngine.setFont(m_font);

	m_maxCacheGlyphs = 2048;// TODO 定数なのはなんとかしたい

	// 横方向に並べる数
	// +1.0 は切り捨て対策。テクスチャサイズはmaxCharactersが収まる大きさであれば良い。
	// (小さくなければOK)
	m_glyphWidthCount = (int)(sqrt((double)m_maxCacheGlyphs) + 1.0);
	int w = m_glyphWidthCount * m_font->getLineSpacing();	//TODO ビットマップが収まるサイズは要チェック

	// キャッシュ用テクスチャ作成
	m_fillGlyphsTexture = newObject<Texture2D>(SizeI(w, w), TextureFormat::R8G8B8A8, false, ResourceUsage::Dynamic);

	// 検索に使う情報をリセット
	m_curPrimUsedFlags.resize(m_maxCacheGlyphs);
	for (int i = 0; i < m_maxCacheGlyphs; i++)
	{
		m_indexStack.push(i);
	}
	resetUsedFlags();
}

//------------------------------------------------------------------------------
void FontGlyphTextureCache::lookupGlyphInfo(UTF32 ch, CacheGlyphInfo* outInfo, bool* outFlush)
{
	int cacheIndex = -1;
	CachedGlyphInfoMap::const_iterator itr = m_cachedGlyphInfoMap.find(ch);
	if (itr != m_cachedGlyphInfoMap.end())
	{
		const CachedGlyphInfo& info = itr->second;
		cacheIndex = info.index;
		//outInfo->fillGlyphBitmap = nullptr;
		outInfo->outlineOffset = 0;
		outInfo->srcRect.set(	// 描画スレッド側で作るといろいろな情報にアクセスしなければならないのでここで作ってしまう
			((info.index % m_glyphWidthCount) * m_glyphMaxBitmapSize.width),
			((info.index / m_glyphWidthCount) * m_glyphMaxBitmapSize.height),
			info.size.width, info.size.height);
	}
	else
	{
		if (m_indexStack.getCount() == 0) {
			// TODO: 古いキャッシュ破棄
			LN_NOTIMPLEMENTED();
		}

		// ビットマップを新しく作ってキャッシュに登録したい
		FontGlyphBitmap* glyphBitmap = m_font->lookupGlyphBitmap(ch, 0);

		// 空いてるインデックスを取りだす
		cacheIndex = m_indexStack.getTop();
		m_indexStack.pop();

		// キャッシュマップに登録
		CachedGlyphInfo info;
		info.index = cacheIndex;
		info.size = glyphBitmap->GlyphBitmap->getSize();
		m_cachedGlyphInfoMap[ch] = info;

		//outInfo->fillGlyphBitmap = glyphBitmap->GlyphBitmap;
		outInfo->outlineOffset = glyphBitmap->OutlineOffset;
		outInfo->srcRect.set(	// 描画スレッド側で作るといろいろな情報にアクセスしなければならないのでここで作ってしまう
			((info.index % m_glyphWidthCount) * m_glyphMaxBitmapSize.width),
			((info.index / m_glyphWidthCount) * m_glyphMaxBitmapSize.height),
			info.size.width, info.size.height);


		// Fill
		PointI pt(outInfo->srcRect.x + outInfo->outlineOffset, outInfo->srcRect.y + outInfo->outlineOffset);
		m_fillGlyphsTexture->blt(pt.x, pt.y, glyphBitmap->GlyphBitmap);
		//m_lockedFillBitmap->bitBlt(dst, info->fillGlyphBitmap, src, Color::White, false);
	}

	// 今回、cacheIndex を使うことをマーク
	if (!m_curPrimUsedFlags[cacheIndex])
	{
		m_curPrimUsedFlags[cacheIndex] = true;
		++m_curPrimUsedCount;
	}

	// キャッシュが一杯になっていないかチェック。
	// 一杯になってたら呼び出し元に Flush してもらわないと、一部の文字が描画できないことになる。
	if (m_curPrimUsedCount == m_maxCacheGlyphs)
	{
		resetUsedFlags();
		(*outFlush) = true;
	}
	else
	{
		(*outFlush) = false;
	}
}

#if 0
//------------------------------------------------------------------------------
void FontGlyphTextureCache::CommitCacheGlyphInfo(CacheGlyphInfo* info, Rect* srcFillRect, Rect* srcOutlineRect)
{
	(*srcFillRect) = info->srcRect;
	(*srcOutlineRect) = Rect(info->srcRect.x, info->srcRect.y, info->srcRect.width + info->outlineOffset, info->srcRect.height + info->outlineOffset);

	if (info->fillGlyphBitmap == nullptr)
	{
	}
	else
	{
		if (m_lockedFillBitmap == nullptr)
		{
			m_lockedFillBitmap = m_glyphsFillTexture->Lock();
		}

		// Fill
		Rect dst(info->srcRect.x + info->outlineOffset, info->srcRect.y + info->outlineOffset, info->fillGlyphBitmap->GetSize());
		Rect src(0, 0, info->fillGlyphBitmap->GetSize());
		m_lockedFillBitmap->BitBlt(dst, info->fillGlyphBitmap, src, Color::White, false);

		//m_lockedFillBitmap->Clear(Color::White);
		// TODO: Outline
	}
}
#endif

//------------------------------------------------------------------------------
Texture2D* FontGlyphTextureCache::getGlyphsFillTexture()
{
	return m_fillGlyphsTexture;
}

////------------------------------------------------------------------------------
//Driver::ITexture* FontGlyphTextureCache::getGlyphsFillTexture()
//{
//	return m_fillGlyphsTexture->resolveDeviceObject();
//}

//------------------------------------------------------------------------------
void FontGlyphTextureCache::onFlush()
{
	resetUsedFlags();
}

//------------------------------------------------------------------------------
//const SizeI& FontGlyphTextureCache::GetGlyphsTextureSize() const
//{
//	return m_fillGlyphsTexture->getRealSize();
//}

//------------------------------------------------------------------------------
void FontGlyphTextureCache::resetUsedFlags()
{
	for (int i = 0; i < m_maxCacheGlyphs; ++i)
	{
		m_curPrimUsedFlags[i] = false;
	}
	m_curPrimUsedCount = 0;
}

#if 0
//------------------------------------------------------------------------------
FontGlyphTextureCache::FontGlyphTextureCache(GraphicsManager* manager, RawFont* font)
{
	m_manager = manager;
	m_font = font;
	m_layoutEngine.SetFont(m_font);

	int maxCharacters = 2048;// TODO 定数なのはなんとかしたい

	// 横方向に並べる数
	// +1.0 は切り捨て対策。テクスチャサイズはmaxCharactersが収まる大きさであれば良い。
	// (小さくなければOK)
	m_glyphWidthCount = (int)(sqrt((double)maxCharacters) + 1.0);
	int w = m_glyphWidthCount * m_font->GetLineSpacing();	//TODO ビットマップが収まるサイズは要チェック

	// キャッシュ用テクスチャ作成
	m_glyphCacheTexture = Texture2D::create(SizeI(w, w), TextureFormat_R8G8B8A8, 1);	// TODO: GraphicsManager?
	//Driver::IGraphicsDevice* device = m_spriteRenderer->getManager()->getGraphicsDevice()->getDeviceObject();
	//m_glyphCacheTexture.Attach(device->createTexture(SizeI(w, w), 1, TextureFormat_R8G8B8A8));


	//Device::IGraphicsDevice::ScopedLockContext lock(m_spriteRenderer->getManager()->getGraphicsDevice()->getDeviceObject());
	//BitmapPainter painter(m_glyphCacheTexture->getDeviceObject()->lock());
	//painter.Clear(Color::Blue);
	//m_glyphCacheTexture->getDeviceObject()->unlock();

	// 空きキャッシュインデックス作成
	for (int i = 0; i < maxCharacters; i++) {
		m_indexStack.push(i);
	}

	m_glyphMaxBitmapSize = m_font->GetGlyphMaxSize();
	m_tmpBitmap.Attach(LN_NEW RawBitmap(m_font->GetGlyphMaxSize(), Utils::TranslatePixelFormat(m_glyphCacheTexture->GetFormat())));
}

//------------------------------------------------------------------------------
void FontGlyphTextureCache::LookupGlyph(UTF32 ch, int strokeThickness, Texture** texture, Rect* srcRect)
{
	// とりあえず今は1つしか無いので
	*texture = m_glyphCacheTexture;

	CachedGlyphInfoMap::iterator itr = m_cachedGlyphInfoMap.find(ch);
	if (itr != m_cachedGlyphInfoMap.end())
	{
		CachedGlyphInfo& info = itr->second;
		srcRect->Set(
			((info.Index % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width),
			((info.Index / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height),
			info.SizeI.Width, info.SizeI.Height);
	}
	else
	{
		if (m_indexStack.GetCount() == 0) {
			// TODO: 古いキャッシュ破棄
			LN_NOTIMPLEMENTED();
		}

		// 新しく作ってキャッシュに登録
		FontGlyphBitmap* glyphBitmap = m_font->LookupGlyphBitmap(ch, strokeThickness);

		int cacheIndex = m_indexStack.getTop();
		m_indexStack.pop();


		// m_tmpBitmap へ bitBlt することで、アウトラインのビットマップと結合し、フォーマットをそろえる

		if (glyphBitmap->OutlineBitmap != NULL)
		{
			m_tmpBitmap->SetSize(glyphBitmap->OutlineBitmap->GetSize());
			Rect rc(0, 0, glyphBitmap->OutlineBitmap->GetSize());
			m_tmpBitmap->BitBlt(rc, glyphBitmap->OutlineBitmap, rc, Color::Green, false);
		}
		else {
			m_tmpBitmap->SetSize(glyphBitmap->GlyphBitmap->GetSize());
		}
		Rect dst(glyphBitmap->OutlineOffset, glyphBitmap->OutlineOffset, glyphBitmap->GlyphBitmap->GetSize());
		Rect src(0, 0, glyphBitmap->GlyphBitmap->GetSize());
		m_tmpBitmap->BitBlt(dst, glyphBitmap->GlyphBitmap, src, Color::Red, false);


		PointI pt(
			(cacheIndex % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width,
			(cacheIndex / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height);
		m_glyphCacheTexture->SetSubData(pt, m_tmpBitmap);

		//Rect destRect(
		//	(cacheIndex % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width,
		//	(cacheIndex / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height,
		//	m_glyphMaxBitmapSize);
		//Rect srcRect(
		//	0, 0,
		//	glyhp->GlyphBitmap->GetSize());
		//lock.GetBitmap()->bitBlt(destRect, glyhp->GlyphBitmap, srcRect, Color::White, false);
		//}

		// キャッシュマップに登録
		CachedGlyphInfo info;
		info.Index = cacheIndex;
		info.SizeI = glyphBitmap->GlyphBitmap->GetSize();
		m_cachedGlyphInfoMap[ch] = info;

		srcRect->Set(
			((info.Index % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width),
			((info.Index / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height),
			info.SizeI.Width, info.SizeI.Height);
	}
}

//------------------------------------------------------------------------------
uint64_t FontGlyphTextureCache::CalcFontSettingHash() const
{
	uint32_t v[2];
	v[0] = Hash::calcHash(m_font->GetName().c_str());

	uint8_t* v2 = (uint8_t*)&v[1];
	v2[0] = m_font->GetSize();
	//v2[1] = m_font->GetEdgeSize();
	v2[3] =
		(((m_font->IsBold()) ? 1 : 0)) |
		(((m_font->IsItalic()) ? 1 : 0) << 1) |
		(((m_font->IsAntiAlias()) ? 1 : 0) << 2);

	return *((uint64_t*)&v);
}
#endif

//------------------------------------------------------------------------------
void FontGlyphTextureCache::measure(const UTF32* text, int length, TextLayoutResult* outResult)
{
	m_layoutEngine.layoutText(text, length, LayoutTextOptions::All, outResult);	// TODO: RenderSize だけでもいいかも？
}



//==============================================================================
// VectorFontGlyphCache
//==============================================================================

//------------------------------------------------------------------------------
VectorFontGlyphCache::VectorFontGlyphCache()
{
}

//------------------------------------------------------------------------------
VectorFontGlyphCache::~VectorFontGlyphCache()
{
}

//------------------------------------------------------------------------------
void VectorFontGlyphCache::initialize(GraphicsManager* manager, RawFont* font, int maxSize)
{
	m_manager = manager;
	m_font = font;
	m_glyphInfoList.resize(maxSize);
	m_inFlushUsedFlags.resize(maxSize);
	m_gryphBufferDataList.resize(maxSize);
	m_freeIndexCount = maxSize;
	for (int i = 0; i < maxSize; i++)
	{
		m_glyphInfoList[i].idIndex = i;
	}

	resetUsedFlags();
}

//------------------------------------------------------------------------------
VectorFontGlyphCache::Handle VectorFontGlyphCache::getGlyphInfo(char32_t utf32Code, bool* outFlushRequested)
{
	int infoIndex = -1;
	auto itr = m_glyphInfoIndexMap.find(utf32Code);
	if (itr != m_glyphInfoIndexMap.end())
	{
		infoIndex = itr->second;
	}
	else
	{
		if (m_freeIndexCount > 0)
		{
			// 空いてるインデックスを取りだす
			infoIndex = m_freeIndexCount - 1;
			m_freeIndexCount--;
			//m_freeIndexStack.GetTop();
			//m_freeIndexStack.Pop();
		}
		else
		{
			// 最も古いものを選択
			GryphInfo* info = m_olderInfoList.popFront();
			infoIndex = info->idIndex;
			m_glyphInfoIndexMap.erase(info->utf32Code);
		}

		// キャッシュマップに登録
		m_glyphInfoIndexMap[utf32Code] = infoIndex;

		RawFont::VectorGlyphInfo info;
		m_font->decomposeOutline(utf32Code, &info);
		
		// レンダリングスレッドへデータを送る
		{
			RenderBulkData vertexList(&info.vertices[0], sizeof(RawFont::FontOutlineVertex) * info.vertices.getCount());
			RenderBulkData outlineList(&info.outlines[0], sizeof(RawFont::OutlineInfo) * info.outlines.getCount());
			VectorFontGlyphCache* this_ = this;
			LN_ENQUEUE_RENDER_COMMAND_4(
				registerPolygons, m_manager,
				VectorFontGlyphCache*, this_,
				Handle, infoIndex,
				RenderBulkData, vertexList,
				RenderBulkData, outlineList,
				{
					this_->registerPolygons(
						infoIndex,
						reinterpret_cast<const RawFont::FontOutlineVertex*>(vertexList.getData()),
						vertexList.getSize() / sizeof(RawFont::FontOutlineVertex),
						reinterpret_cast<const RawFont::OutlineInfo*>(outlineList.getData()),
						outlineList.getSize() / sizeof(RawFont::OutlineInfo));
				});
		}
	}

	GryphInfo* info = &m_glyphInfoList[infoIndex];
	info->utf32Code = utf32Code;

	// 最新とする
	m_olderInfoList.remove(info);
	m_olderInfoList.add(info);

	// 今回、cacheIndex を使うことをマーク
	if (!m_inFlushUsedFlags[infoIndex])
	{
		m_inFlushUsedFlags[infoIndex] = true;
		m_inFlushUsedCount++;
	}

	// キャッシュが一杯になっていないかチェック。
	// 一杯になってたら、異なる文字が max 個書かれようとしているということ。
	// 呼び出し元に Flush してもらわないと、一部の文字が描画できないことになる。
	if (m_inFlushUsedCount == getMaxCount())
	{
		resetUsedFlags();
		*outFlushRequested = true;
	}
	else
	{
		*outFlushRequested = false;
	}

	return infoIndex;
}

//------------------------------------------------------------------------------
void VectorFontGlyphCache::onFlush()
{
	resetUsedFlags();
}

//------------------------------------------------------------------------------
int VectorFontGlyphCache::getVertexCount(Handle info)
{
	return m_gryphBufferDataList[info].vertices.getCount();
}

//------------------------------------------------------------------------------
int VectorFontGlyphCache::getIndexCount(Handle info)
{
	return m_gryphBufferDataList[info].triangleIndices.getCount();
}

//------------------------------------------------------------------------------
void VectorFontGlyphCache::generateMesh(Handle infoIndex, const Vector3& baselineOrigin, const Matrix& transform, Vertex* outVertices, uint16_t* outIndices, uint16_t beginIndex)
{
	auto* info = &m_gryphBufferDataList[infoIndex];
	bool isIdent = transform.isIdentity();
	for (int i = 0; i < info->vertices.getCount(); i++)
	{
		outVertices[i].position = Vector3(info->vertices[i].pos, 0.0f);
		outVertices[i].position.y *= -1;
		outVertices[i].color = Color(0.25, 0.25, 0.25, info->vertices[i].alpha);

		if (!isIdent) outVertices[i].position.transformCoord(transform);
		outVertices[i].position += baselineOrigin;
	}
	for (int i = 0; i < info->triangleIndices.getCount(); i++)
	{
		outIndices[i] = beginIndex + info->triangleIndices[i];
	}
}

//------------------------------------------------------------------------------
void VectorFontGlyphCache::resetUsedFlags()
{
	for (int i = 0; i < getMaxCount(); i++)
	{
		m_inFlushUsedFlags[i] = false;
	}
	m_inFlushUsedCount = 0;
}

//------------------------------------------------------------------------------
void VectorFontGlyphCache::registerPolygons(Handle infoIndex, const RawFont::FontOutlineVertex* vertices, int vertexSize, const RawFont::OutlineInfo* outlines, int outlineSize)
{

	auto* info = &m_gryphBufferDataList[infoIndex];

	// TODO: AddRange
	info->vertices.clear();
	info->vertices.reserve(vertexSize);
	for (int i = 0; i < vertexSize; i++)
	{
		info->vertices.add(vertices[i]);
	}
	info->outlines.clear();
	info->outlines.reserve(outlineSize);
	for (int i = 0; i < outlineSize; i++)
	{
		info->outlines.add(outlines[i]);
	}

	FontOutlineTessellator tess;	// TODO: インスタンスはメンバに持っておいたほうが malloc 少なくなっていいかな？
	tess.tessellate(info);

	FontOutlineStroker stroker;
	stroker.makeStroke(info);
}

} // namespace detail
LN_NAMESPACE_END


#if 0
namespace detail
{


	//class CharTextureCache
	//{
	//private:
	//	struct CachedGlyphInfo
	//	{
	//		int		Index;
	//		SizeI	SizeI;
	//	};

	//	typedef std::map<UTF32, CachedGlyphInfo> CachedGlyphInfoMap;

	//	CachedGlyphInfoMap			m_cachedGlyphInfoMap;
	//	Ref<Device::ITexture>	m_glyphCacheTexture;
	//	Ref<RawFont>		m_font;


	//public:

	//	CharTextureCache()
	//		: m_cachedGlyphInfoMap()
	//		, m_glyphCacheTexture()
	//		, m_font(NULL)
	//	{
	//	}

	//	void setFont(RawFont* font)
	//	{
	//		m_font = font;
	//	}

	//	void DrawCharSprite(UTF32 ch)
	//	{

	//	}
	//};












//==============================================================================
// TextRendererImplemented
//==============================================================================

//------------------------------------------------------------------------------
TextRendererImplemented::TextRendererImplemented(GraphicsManager* manager)
{
	int maxCharacters = 2048;// TODO 定数なのはなんとかしたい
	m_spriteRenderer.Attach(LN_NEW SpriteRendererImpl(manager, maxCharacters));
}

//------------------------------------------------------------------------------
TextRendererImplemented::~TextRendererImplemented()
{
}

//------------------------------------------------------------------------------
void TextRendererImplemented::DrawChar(UTF32 ch, const Rect& area)
{
	CachedGlyphInfoMap::iterator itr = m_cachedGlyphInfoMap.find(ch);
	if (itr != m_cachedGlyphInfoMap.end())
	{
		// 描画
		DrawSprite(itr->second, area.X, area.Y);
	}
	else
	{
		if (m_indexStack.GetCount() == 0) {
			// TODO: 古いキャッシュ破棄
			LN_NOTIMPLEMENTED();
		}

		// 新しく作ってキャッシュに登録
		FontGlyphData* glyhp = m_font->LookupGlyphData(ch, NULL);

		int cacheIndex = m_indexStack.getTop();
		m_indexStack.pop();

		{
			Driver::ITexture::ScopedLock lock(m_glyphCacheTexture);

			Rect destRect(
				(cacheIndex % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width,
				(cacheIndex / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height,
				m_glyphMaxBitmapSize);
			Rect srcRect(
				0, 0,
				glyhp->GlyphBitmap->GetSize());
			lock.GetBitmap()->BitBlt(destRect, glyhp->GlyphBitmap, srcRect, Color::White, false);
		}

		// キャッシュマップに登録
		CachedGlyphInfo e;
		e.Index = cacheIndex;
		e.SizeI = glyhp->GlyphBitmap->GetSize();
		m_cachedGlyphInfoMap[ch] = e;

		// 描画
		DrawSprite(e, area.X, area.Y);
	}
}

//------------------------------------------------------------------------------
void TextRendererImplemented::Flash()
{

}

//------------------------------------------------------------------------------
void TextRendererImplemented::DrawSprite(const CachedGlyphInfo& info, int x, int y)
{
	RectF srcRect(
		(float)((info.Index % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width),
		(float)((info.Index / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height),
		(float)info.SizeI.Width, (float)info.SizeI.Height);

	const ColorF c[4] = { ColorF::White, ColorF::White, ColorF::White, ColorF::White };
	m_spriteRenderer->DrawRequest2D(
		Vector3(x, y, 0),
		Vector3::Zero,
		Vector2((float)info.SizeI.Width, (float)info.SizeI.Height),
		m_glyphCacheTexture,
		srcRect,
		c);
}

//------------------------------------------------------------------------------
void TextRendererImplemented::Reset()
{
	int maxCharacters = 2048;// TODO 定数なのはなんとかしたい

	// 横方向に並べる数
	// +1.0 は切り捨て対策。テクスチャサイズはmaxCharactersが収まる大きさであれば良い。
	// (小さくなければOK)
	m_glyphWidthCount = (int)(sqrt((double)maxCharacters) + 1.0);
	int w = m_glyphWidthCount * m_font->GetLineHeight();	//TODO ビットマップが収まるサイズは要チェック

	// キャッシュ用テクスチャ作成
	Device::IGraphicsDevice* device = m_spriteRenderer->GetManager()->GetGraphicsDevice();
	m_glyphCacheTexture.Attach(device->CreateTexture(SizeI(w, w), 1, TextureFormat_R8G8B8A8));

	// 空きキャッシュインデックス作成
	for (int i = 0; i < maxCharacters; i++) {
		m_indexStack.push(i);
	}

	m_glyphMaxBitmapSize = m_font->GetGlyphMaxSize();
}

} // namespace detail

#endif

#if 0
//==============================================================================
// TextRenderer
//==============================================================================

//------------------------------------------------------------------------------
TextRenderer* TextRenderer::create(GraphicsManager* manager)
{
	return LN_NEW TextRenderer(manager);
}

//------------------------------------------------------------------------------
TextRenderer::TextRenderer(GraphicsManager* manager)
	: m_spriteRenderer()
	, m_font()
	, m_foreColor()
	, m_strokeColor()
	, m_strokeSize(0)
	//, m_textAlignment(TextAlignment::Left)
	//, m_textTrimming(TextTrimming::None)
	//, m_flowDirection(FlowDirection::LeftToRight)
	, m_prevGlyphLocationData(NULL)
	, m_fontFaceModified(true)
{
	//m_manager = manager;
	//m_impl = LN_NEW TextRendererImplemented(manager);
	m_spriteRenderer.Attach(SpriteRenderer::create(2048, manager));	// TODO 定数
}

//------------------------------------------------------------------------------
TextRenderer::~TextRenderer()
{
	//LN_SAFE_RELEASE(m_impl);
}

//------------------------------------------------------------------------------
void TextRenderer::SetTransform(const Matrix& matrix)
{
	m_spriteRenderer->SetTransform(matrix);
}

//------------------------------------------------------------------------------
void TextRenderer::SetViewProjection(const Matrix& view, const Matrix& proj, const SizeI& viewPixelSize)
{
	m_spriteRenderer->SetViewProjMatrix(view, proj);
	m_spriteRenderer->SetViewPixelSize(viewPixelSize);
}

//------------------------------------------------------------------------------
void TextRenderer::Measure(const UTF32* text, int length, TextLayoutResult* outResult)
{
	m_layoutEngine.LayoutText(text, length, outResult);
}

//------------------------------------------------------------------------------
void TextRenderer::DrawText(const char* text, int length)
{
	length = (length < 0) ? strlen(text) : length;
	EncodingConversionResult result;
	const ByteBuffer& utf32Buf = m_font->GetManager()->GetCharToUTF32Converter()->Convert(text, length * sizeof(char), &result);
	const UTF32* utf32 = (const UTF32*)utf32Buf.GetConstData();
	for (size_t i = 0; i < result.CharsUsed; ++i)
	{
		DrawChar(utf32[i]);
	}
}

//------------------------------------------------------------------------------
void TextRenderer::DrawText(const wchar_t* text, int length)
{
	length = (length < 0) ? wcslen(text) : length;
	EncodingConversionResult result;
	const ByteBuffer& utf32Buf = m_font->GetManager()->GetWCharToUTF32Converter()->Convert(text, length * sizeof(wchar_t), &result);
	const UTF32* utf32 = (const UTF32*)utf32Buf.GetConstData();
	for (size_t i = 0; i < result.CharsUsed; ++i)
	{
		DrawChar(utf32[i]);
	}
}

//------------------------------------------------------------------------------
void TextRenderer::Flash()
{
	m_spriteRenderer->Flash();
}

//------------------------------------------------------------------------------
void TextRenderer::DrawChar(UTF32 ch)
{
	CheckResetCache();
	m_prevGlyphLocationData = m_font->AdvanceKerning(ch, m_prevGlyphLocationData);

	CachedGlyphInfoMap::iterator itr = m_cachedGlyphInfoMap.find(ch);
	if (itr != m_cachedGlyphInfoMap.end())
	{
		// 描画
		DrawSprite(itr->second, m_prevGlyphLocationData->OutlineBitmapTopLeftPosition);
	}
	else
	{
		if (m_indexStack.GetCount() == 0) {
			// TODO: 古いキャッシュ破棄
			LN_NOTIMPLEMENTED();
		}

		// 新しく作ってキャッシュに登録
		//FontGlyphData* glyhp = m_font->LookupGlyphData(ch, NULL);
		FontGlyphBitmap* glyphBitmap = m_font->LookupGlyphBitmap(ch);

		int cacheIndex = m_indexStack.getTop();
		m_indexStack.pop();

		//m_tmpBitmap

		//{
		//	Device::ITexture::ScopedLock lock(m_glyphCacheTexture);

		// m_tmpBitmap へ bitBlt することで、アウトラインのビットマップと結合し、フォーマットをそろえる

		if (glyphBitmap->OutlineBitmap != NULL)
		{
			m_tmpBitmap->SetSize(glyphBitmap->OutlineBitmap->GetSize());
			Rect rc(0, 0, glyphBitmap->OutlineBitmap->GetSize());
			m_tmpBitmap->BitBlt(rc, glyphBitmap->OutlineBitmap, rc, Color::Green, false);
		}
		else {
			m_tmpBitmap->SetSize(glyphBitmap->GlyphBitmap->GetSize());
		}
		Rect dst(glyphBitmap->OutlineOffset, glyphBitmap->OutlineOffset, glyphBitmap->GlyphBitmap->GetSize());
		Rect src(0, 0, glyphBitmap->GlyphBitmap->GetSize());
		m_tmpBitmap->BitBlt(dst, glyphBitmap->GlyphBitmap, src, Color::Red, false);
		//BitmapPainter painter(m_tmpBitmap);
		//painter.Clear(Color::Blue);




		PointI pt(
			(cacheIndex % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width,
			(cacheIndex / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height);
		m_glyphCacheTexture->SetSubData(pt, m_tmpBitmap);

			//Rect destRect(
			//	(cacheIndex % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width,
			//	(cacheIndex / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height,
			//	m_glyphMaxBitmapSize);
			//Rect srcRect(
			//	0, 0,
			//	glyhp->GlyphBitmap->GetSize());
			//lock.GetBitmap()->bitBlt(destRect, glyhp->GlyphBitmap, srcRect, Color::White, false);
		//}

		// キャッシュマップに登録
		CachedGlyphInfo e;
		e.Index = cacheIndex;
		e.SizeI = glyphBitmap->GlyphBitmap->GetSize();
		m_cachedGlyphInfoMap[ch] = e;

		// 描画
		DrawSprite(e, m_prevGlyphLocationData->OuterTopLeftPosition);
	}
}

//------------------------------------------------------------------------------
void TextRenderer::DrawSprite(const CachedGlyphInfo& info, const PointI& point)
{
	RectF srcRect(
		(float)((info.Index % m_glyphWidthCount) * m_glyphMaxBitmapSize.Width),
		(float)((info.Index / m_glyphWidthCount) * m_glyphMaxBitmapSize.Height),
		(float)info.SizeI.Width, (float)info.SizeI.Height);

	const ColorF c[4] = { ColorF::White, ColorF::White, ColorF::White, ColorF::White };
	m_spriteRenderer->DrawRequest2D(
		Vector3(point.X, point.Y, 0),
		Vector3::Zero,
		Vector2(info.SizeI.Width, info.SizeI.Height),
		m_glyphCacheTexture,
		srcRect,
		c);
}

//------------------------------------------------------------------------------
void TextRenderer::CheckResetCache()
{
	if (m_fontFaceModified)
	{
		int maxCharacters = 2048;// TODO 定数なのはなんとかしたい

		// 横方向に並べる数
		// +1.0 は切り捨て対策。テクスチャサイズはmaxCharactersが収まる大きさであれば良い。
		// (小さくなければOK)
		m_glyphWidthCount = (int)(sqrt((double)maxCharacters) + 1.0);
		int w = m_glyphWidthCount * m_font->GetLineHeight();	//TODO ビットマップが収まるサイズは要チェック

		// キャッシュ用テクスチャ作成
		m_glyphCacheTexture.Attach(Texture::create(SizeI(w, w), TextureFormat_R8G8B8A8, 1, m_spriteRenderer->GetManager()));
		//Device::IGraphicsDevice* device = m_spriteRenderer->getManager()->getGraphicsDevice()->getDeviceObject();
		//m_glyphCacheTexture.Attach(device->createTexture(SizeI(w, w), 1, TextureFormat_R8G8B8A8));


		//Device::IGraphicsDevice::ScopedLockContext lock(m_spriteRenderer->getManager()->getGraphicsDevice()->getDeviceObject());
		//BitmapPainter painter(m_glyphCacheTexture->getDeviceObject()->lock());
		//painter.Clear(Color::Blue);
		//m_glyphCacheTexture->getDeviceObject()->unlock();

		// 空きキャッシュインデックス作成
		for (int i = 0; i < maxCharacters; i++) {
			m_indexStack.push(i);
		}

		m_glyphMaxBitmapSize = m_font->GetGlyphMaxSize();
		m_tmpBitmap.Attach(LN_NEW RawBitmap(m_font->GetGlyphMaxSize(), Utils::TranslatePixelFormat(m_glyphCacheTexture->GetFormat())));


		m_prevGlyphLocationData = NULL;
		m_fontFaceModified = false;
	}
}
#endif
