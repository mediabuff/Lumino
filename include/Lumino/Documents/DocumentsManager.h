﻿
#pragma once
#include "../Graphics/Font.h"
#include "../Graphics/GraphicsManager.h"

namespace Lumino
{
namespace Documents
{

//class GlyphCache
//{
//public:
//
//};
//
//class GlyphCacheItem
//{
//public:
//
//};

/**
	@brief
*/
class DocumentsManager
	: public RefObject
{
public:
	struct ConfigData
	{
		Graphics::GraphicsManager*	GraphicsManager;

		ConfigData()
			: GraphicsManager(NULL)
		{}
	};

public:
	DocumentsManager(const ConfigData& configData);
	virtual ~DocumentsManager();

public:
	Graphics::GraphicsManager* GetGraphicsManager() const { return m_graphicsManager; }
	EncodingConverter* GetTCharToUTF32Converter() { return &m_TCharToUTF32Converter; }	// TODO: fontmanager の使えばいいかも
	EncodingConverter* GetUTF32ToTCharConverter() { return &m_UTF32ToTCharConverter; }
	
private:
	Graphics::GraphicsManager*	m_graphicsManager;
	EncodingConverter		m_TCharToUTF32Converter;
	EncodingConverter		m_UTF32ToTCharConverter;
};


} // namespace Documents
} // namespace Lumino
