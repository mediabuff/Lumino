﻿
#pragma once
#include "UIElement.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GUI_BEGIN
	
/**
	@brief	ContentControl のコンテンツを表示します。
*/
class ContentPresenter
	: public UIElement
{
	LN_CORE_OBJECT_TYPE_INFO_DECL();
	LN_UI_ELEMENT_SUBCLASS_DECL(ContentPresenter);
public:
	
	/**
		@brief	ContentPresenter を作成します。
	*/
	static ContentPresenterPtr Create();

	ContentPresenter(GUIManagerImpl* manager);
	virtual ~ContentPresenter();

	void SetContent(UIElement* content);

protected:
	virtual int GetVisualChildrenCount() const;
	virtual UIElement* GetVisualChild(int index) const;
#if 1
	//virtual void ApplyTemplateHierarchy(CombinedLocalResource* parent);
	virtual SizeF MeasureOverride(const SizeF& constraint);
	virtual SizeF ArrangeOverride(const SizeF& finalSize);
#endif

	// TODO: InvalidateArrange/Measure

LN_INTERNAL_ACCESS:
	void SetOwner(ContentControl* owner);

private:
	ContentControl*		m_owner;
	RefPtr<UIElement>	m_content;
	//virtual void AddVisualChild(UIElement* child) { LN_THROW(0, InvalidOperationException); }	// ContentPresenter は論理的な子要素の配置先をマークするメタデータのようなものなので、子要素は持たない。
};

LN_NAMESPACE_GUI_END
LN_NAMESPACE_END
