﻿
#pragma once
#include "../../CoreObjectList.h"
#include "../UIElement.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GUI_BEGIN

/// コンテンツサイズの計算方法
LN_ENUM(Stretch)
{
	None = 0,
};
LN_ENUM_DECLARE(Stretch);

/**
	@brief
*/
class Image
	: public UIElement
{
	LN_CORE_OBJECT_TYPE_INFO_DECL();
	LN_UI_ELEMENT_SUBCLASS_DECL(Image);
public:
	LN_PROPERTY(Variant,	SourceProperty);		// TODO: OnSourcePropertyChagend。あともしかしたら Converter がほしいかも
	LN_PROPERTY(Rect,		SourceRectProperty);

public:
	static ImagePtr Create();

	Image(GUIManagerImpl* manager);
	virtual ~Image();

	//-------------------------------------------------------------------------
	/** @name Properties */
	/** @{ */

	void SetSource(const Variant& value);

	void SetSourceRect(const Rect& rect);
	const Rect& GetSourceRect() const { return m_srcRect; }

	// ImageSource Source
	// Stretch Stretch

	/** @} */

	//-------------------------------------------------------------------------
	/** @name Events */
	/** @{ */
	

	/** @} */

public:
	void SetSourceTexture(Texture* texture);

protected:
	virtual SizeF MeasureOverride(const SizeF& availableSize);
	virtual SizeF ArrangeOverride(const SizeF& finalSize);
	virtual void OnRender(RenderingContext* painter);

private:
	void UpdateInternalSourceRect();

private:
	Variant	m_source;

	RefPtr<Texture>	m_texture;
	Rect						m_srcRect;
	Stretch						m_stretch;
	Rect						m_internalSrcRect;
};


LN_NAMESPACE_GUI_END
LN_NAMESPACE_END
