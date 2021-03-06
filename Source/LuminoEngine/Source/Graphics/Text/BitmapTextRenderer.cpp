﻿
#include "../Internal.h"
#include <Lumino/Graphics/Bitmap.h>
#include <Lumino/Graphics/Text/GlyphRun.h>
#include "BitmapTextRenderer.h"
#include "../GraphicsManager.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN

//==============================================================================
// BitmapTextRenderer
//==============================================================================

//------------------------------------------------------------------------------
BitmapTextRenderer::BitmapTextRenderer()
	: m_manager(nullptr)
	, m_tmpGlyphRun(nullptr)
	, m_textAlignment(TextAlignment::Left)
{
}

//------------------------------------------------------------------------------
BitmapTextRenderer::~BitmapTextRenderer()
{
	LN_SAFE_RELEASE(m_tmpGlyphRun);
}

//------------------------------------------------------------------------------
void BitmapTextRenderer::initialize(detail::GraphicsManager* manager)
{
	if (LN_REQUIRE(manager != nullptr)) return;
	m_manager = manager;

	m_tmpGlyphRun = LN_NEW GlyphRun();
	m_tmpGlyphRun->initialize(m_manager);
}

//------------------------------------------------------------------------------
void BitmapTextRenderer::drawGlyphRun(RawBitmap* target, GlyphRun* glyphRun, const Color32& fillColor, const Color32& strokeColor, int strokeThickness)
{
	if (LN_REQUIRE(target != nullptr)) return;
	if (LN_REQUIRE(glyphRun != nullptr)) return;

	RawFont* font = glyphRun->getFont();

	auto& items = glyphRun->requestLayoutItems();
	auto& renderSize = glyphRun->getRenderSize();

	PointI offset(0, 0);
	switch (m_textAlignment)
	{
		case TextAlignment::Left:
		{
			break;
		}
		case TextAlignment::Center:
		{
			offset.x = (m_renderArea.width - renderSize.width) / 2;
			break;
		}
		case TextAlignment::Right:
		{
			offset.x = m_renderArea.getRight() - renderSize.width;
			break;
		}
		case TextAlignment::Justify:
			LN_NOTIMPLEMENTED();
			break;
	}


	PointI pos(m_renderArea.x + offset.x, m_renderArea.y + offset.y);
	for (auto& item : items)
	{
		FontGlyphBitmap* gb = font->lookupGlyphBitmap(item.Char, strokeThickness);
		RectI dstRect;
		RectI srcRect;

		// 枠線用ビットマップがある場合は先に描画する
		if (gb->OutlineBitmap != nullptr)
		{
			dstRect.set(
				pos.x + item.Location.OutlineBitmapTopLeftPosition.x,
				pos.y + item.Location.OutlineBitmapTopLeftPosition.y,
				item.Location.BitmapSize.width,
				item.Location.BitmapSize.height);
			srcRect.set(0, 0, gb->OutlineBitmap->getSize());
			target->bitBlt(dstRect, gb->OutlineBitmap, srcRect, strokeColor, true);
		}

		// 内側 (or 通常) 部分の描画
		if (gb->GlyphBitmap != nullptr)
		{
			dstRect.set(
				pos.x + item.Location.BitmapTopLeftPosition.x,
				pos.y + item.Location.BitmapTopLeftPosition.y,
				item.Location.BitmapSize.width,
				item.Location.BitmapSize.height);
			srcRect.set(0, 0, gb->GlyphBitmap->getSize());
			target->bitBlt(dstRect, gb->GlyphBitmap, srcRect, fillColor, true);
		}
	}
}

LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
