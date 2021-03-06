﻿
#include "Internal.h"
#include "UIManagerImpl.h"
#include <Lumino/GUI/UIButton.h>

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GUI_BEGIN

//=============================================================================
// UIButton
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(UIButton, ContentControl);
LN_UI_ELEMENT_SUBCLASS_IMPL(UIButton);

// Register routed event
LN_ROUTED_EVENT_IMPLEMENT(UIButton, RoutedEventArgs, ClickEvent, "Click", Click);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
UIButton* UIButton::Create(UIElement* parent, const Variant& content)
{
	auto* obj = internalCreateInstance(GetUIManager());
	AutoReleasePool::AddObjectToCurrent(obj);
	if (parent != nullptr) {
		parent->AddChild(obj);
	}
	obj->SetContent(content);
	return obj;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
UIButton::UIButton(GUIManagerImpl* manager)
	: ContentControl(manager)
{
	// Register handler
	IsEnabledChanged += CreateDelegate(this, &UIButton::Handler_IsEnabledChanged);
	LN_REGISTER_ROUTED_EVENT_HANDLER(UIButton, MouseEventArgs, UIElement::MouseEnterEvent, RoutedHandler_MouseEnter);
	LN_REGISTER_ROUTED_EVENT_HANDLER(UIButton, MouseEventArgs, UIElement::MouseLeaveEvent, RoutedHandler_MouseLeave);
	LN_REGISTER_ROUTED_EVENT_HANDLER(UIButton, MouseEventArgs, UIElement::MouseMoveEvent, RoutedHandler_MouseMove);
	LN_REGISTER_ROUTED_EVENT_HANDLER(UIButton, MouseEventArgs, UIElement::MouseDownEvent, RoutedHandler_MouseDown);
	LN_REGISTER_ROUTED_EVENT_HANDLER(UIButton, MouseEventArgs, UIElement::MouseUpEvent, RoutedHandler_MouseUp);

	// TODO: 初期値の再設定は MetadataOverride というメソッドを設ける。
	// 今はまだリセット未対応なので問題ないが。
	SetHorizontalContentAlignment(HorizontalAlignment::Center);
	SetVerticalContentAlignment(VerticalAlignment::Center);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
UIButton::~UIButton()
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::OnClick()
{
	RefPtr<RoutedEventArgs> args(m_manager->GetEventArgsPool()->Create<RoutedEventArgs>(), false);
	RaiseEvent(ClickEvent, this, args);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::OnRender(RenderingContext* painter)
{
	ContentControl::OnRender(painter);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::Handler_IsEnabledChanged(PropertyChangedEventArgs* e)
{
	if (e->NewValue.GetBool()) {
		VisualStateManager::GoToState(this, VisualStatus::Normal);
	}
	else {
		VisualStateManager::GoToState(this, VisualStatus::Disabled);	// TODO: 前の値に戻さなくて良い？
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::RoutedHandler_MouseEnter(MouseEventArgs* e)
{
	VisualStateManager::GoToState(this, VisualStatus::MouseOver);
	e->Handled = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::RoutedHandler_MouseLeave(MouseEventArgs* e)
{
	VisualStateManager::GoToState(this, VisualStatus::Normal);
	e->Handled = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::RoutedHandler_MouseMove(MouseEventArgs* e)
{
	//VisualStateManager::GoToState(this, VisualStatus::MouseOver);

	// TODO
	//OnPropertyChanged(IsMouseOverProperty, m_isMouseOver);
	e->Handled = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::RoutedHandler_MouseDown(MouseEventArgs* e)
{
	Focus();
	VisualStateManager::GoToState(this, VisualStatus::Pressed);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UIButton::RoutedHandler_MouseUp(MouseEventArgs* e)
{
	VisualStateManager::GoToState(this, VisualStatus::Normal);
	OnClick();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void UIButton::UIElement_MouseMove(CoreObject* sender, MouseEventArgs* e)
//{
//	m_isMouseOver = true;
//	OnPropertyChanged(IsMouseOverProperty, m_isMouseOver);
//
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void UIButton::Render()
//{
//	printf("UIButton::Render()\n");
//	ContentControl::Render();
//}

LN_NAMESPACE_GUI_END
LN_NAMESPACE_END
