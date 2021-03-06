﻿
#include "../Internal.h"
#include <Lumino/GUI/ControlTemplate.h>
#include <Lumino/GUI/Controls/Image.h>
#include "../UIManagerImpl.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GUI_BEGIN

//=============================================================================
// Image
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(Image, UIElement);
LN_UI_ELEMENT_SUBCLASS_IMPL(Image);

// Register property
LN_PROPERTY_IMPLEMENT(Image, Variant, SourceProperty, "Source", m_source, PropertyMetadata(Variant::Null));
LN_PROPERTY_IMPLEMENT(Image, Rect, SourceRectProperty, "SourceRect", m_srcRect, PropertyMetadata(Rect::Zero));

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ImagePtr Image::Create()
{
	return ImagePtr(internalCreateInstance(GetUIManager()), false);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Image::Image(GUIManagerImpl* manager)
	: UIElement(manager)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Image::~Image()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Image::SetSource(const Variant& value)
{
	if (value.GetType() == VariantType_Object) 
	{
		auto tex = dynamic_cast<Texture*>(value.GetObject());
		if (tex != NULL)
		{
			SetSourceTexture(tex);
			return;
		}
	}

	LN_CHECK_ARGS(0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Image::SetSourceTexture(Texture* texture)
{
	m_texture = texture;
	UpdateInternalSourceRect();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Image::SetSourceRect(const Rect& rect)
{
	m_srcRect = rect;
	UpdateInternalSourceRect();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Image::UpdateInternalSourceRect()
{
	if (m_texture != NULL)
	{
		if (m_srcRect.IsZero())
		{
			// 転送元矩形が指定されていなければテクスチャサイズを使用する
			const Size& size = m_texture->GetSize();
			m_internalSrcRect.Set(0, 0, size.Width, size.Height);
		}
		else {
			m_internalSrcRect = m_srcRect;
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SizeF Image::MeasureOverride(const SizeF& availableSize)
{
	SizeF desiredSize = UIElement::MeasureOverride(availableSize);

	if (m_stretch == Stretch::None)
	{
		// テクスチャ転送サイズをコントロールの最小サイズにする
		desiredSize.Width = std::max(desiredSize.Width, (float)m_internalSrcRect.Width);
		desiredSize.Height = std::max(desiredSize.Height, (float)m_internalSrcRect.Height);
	}
	return desiredSize;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
SizeF Image::ArrangeOverride(const SizeF& finalSize)
{
	SizeF size = UIElement::ArrangeOverride(finalSize);
	if (m_stretch == Stretch::None)
	{
		size.Width = std::min(size.Width, (float)m_internalSrcRect.Width);
		size.Height = std::min(size.Height, (float)m_internalSrcRect.Height);
	}
	return size;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Image::OnRender(RenderingContext* painter)
{
	if (m_texture == NULL) { return; }

	painter->SetTexture(m_texture, m_internalSrcRect);
	painter->DrawRectangle(m_finalLocalRect);
}

LN_NAMESPACE_GUI_END
LN_NAMESPACE_END

