﻿
#include "../Internal.h"
#include <Lumino/Documents/DocumentsManager.h>
#include <Lumino/Documents/Run.h>
#include "../Graphics/FontGlyphTextureCache.h"
#include "../Graphics/GraphicsHelper.h"

namespace Lumino
{
namespace Documents
{

//=============================================================================
// Run
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(Run, Inline);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Run::Run(DocumentsManager* manager)
	: Inline(manager)
{
	m_glyphRun.Attach(LN_NEW Graphics::GlyphRun());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Run::Run(const String& text, DocumentsManager* manager)
	: Inline(manager)
	, m_text()
{
	m_glyphRun.Attach(LN_NEW Graphics::GlyphRun());
	SetText(text);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Run::~Run()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Run::SetText(const String& text)
{
	m_text.Clear();
	m_text.Append(m_manager->GetTCharToUTF32Converter()->Convert(text.GetCStr(), text.GetByteCount()));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Size Run::Measure()
{
	if (m_fontDataModified)
	{
		UpdateFontData();

		RefPtr<Graphics::Internal::FontGlyphTextureCache> cache = m_manager->GetGraphicsManager()->LookupGlyphTextureCache(m_fontData);
		Graphics::Helper::AttachGlyphTextureCache(m_glyphRun, cache);
	}

	auto* glyphData = Graphics::Helper::GetGlyphData(m_glyphRun);
	auto* cache = Graphics::Helper::GetGlyphTextureCache(m_glyphRun);

	cache->Measure(m_text.GetCStr(), m_text.GetLength(), glyphData);
	return glyphData->AreaSize;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Run::Render(DocumentsRenderer* renderer)
{
	renderer->OnDrawGlyphRun(m_glyphRun, Point::Zero);	// TODO: 位置
}

} // namespace Documents
} // namespace Lumino

