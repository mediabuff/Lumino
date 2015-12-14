
#include "../Internal.h"
#include <Lumino/Graphics/Texture.h>
#include <Lumino/TileMap/TileSet.h>

LN_NAMESPACE_BEGIN

//=============================================================================
// TileSet
//=============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(TileSet, tr::ReflectionObject);
	
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TileSet::TileSet()
	: m_imageSource(nullptr)
	, m_tileCountH(8)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TileSet::~TileSet()
{
	LN_SAFE_RELEASE(m_imageSource);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TileSet::SetImageSource(Texture* texture)
{
	LN_REFOBJ_SET(m_imageSource, texture);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Texture* TileSet::GetImageSource() const
{
	return m_imageSource;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TileSet::LookupTileImage(int id, Texture** outTexture, Rect* outSrcRect)
{
	int x = id % m_tileCountH;
	int y = id / m_tileCountH;
	outSrcRect->Set(x * m_tileSize.Width, y * m_tileSize.Height, m_tileSize.Width, m_tileSize.Height);
	*outTexture = m_imageSource;
}

LN_NAMESPACE_END