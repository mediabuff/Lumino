﻿
#include "../Internal.h"
#include "FontImpl.h"
#include <Lumino/Imaging/Font.h>

namespace Lumino
{
namespace Imaging
{

//=============================================================================
// Font
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Font* Font::CreateBitmapFont(FileManager* fileManager, Bitmap* bitmap)
{
	return LN_NEW BitmapFontImpl(bitmap);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Font::Font()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Font::~Font()
{
}

//=============================================================================
// BitmapFontImpl
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BitmapFontImpl::BitmapFontImpl(Bitmap* bitmap)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BitmapFontImpl::~BitmapFontImpl()
{
}

} // namespace Imaging
} // namespace Lumino
