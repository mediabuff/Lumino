﻿
#include "../Internal.h"
#include "FontManagerImpl.h"

namespace Lumino
{
namespace Imaging
{

//=============================================================================
// FontManager
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FontManager* FontManager::Create(FileManager* fileManager)
{
	return LN_NEW FontManagerImpl(fileManager);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FontManager::FontManager()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FontManager::~FontManager()
{
}

//=============================================================================
// FontManagerImpl
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FontManagerImpl::FontManagerImpl(FileManager* fileManager)
	: m_fileManager(fileManager)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FontManagerImpl::~FontManagerImpl()
{
}

} // namespace Imaging
} // namespace Lumino
