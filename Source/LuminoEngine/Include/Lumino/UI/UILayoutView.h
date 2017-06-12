﻿
#pragma once
#include "UIInjectedInputReceiver.h"
#include "UIItemsControl.h"

LN_NAMESPACE_BEGIN
class DrawingContext;
class PlatformWindow;
class UIPopup;

namespace detail {


class UIPopuoContainer
	: public Object
{
public:
	void SetPopup(UIPopup* popup);
	UIPopup* GetPopup() const;

LN_CONSTRUCT_ACCESS:
	UIPopuoContainer();
	virtual ~UIPopuoContainer();
	void initialize();

private:
	RefPtr<UIPopup>	m_popup;
};

} // namespace detail

/**
	@brief		
	@details	UILayoutView はネイティブウィンドウと UI システムの接合点となり、
				UI レイアウトのルート要素となります。
*/
class UILayoutView
	: public UIControl
	, public IUIInjectedInputReceiver
{
public:

	/** この要素が関連付けられている UIContext を取得します。*/
	UIContext* GetOwnerContext() const { return m_ownerContext; }



LN_INTERNAL_ACCESS:
	void UpdateLayout(const Size& viewSize);
	void Render(DrawingContext* g);
	
	const Size& GetViewPixelSize() const { return m_viewPixelSize; }
	bool UpdateMouseHover(const PointF& mousePos);
	//void CaptureMouse(UIElement* element);
	//void ReleaseMouseCapture(UIElement* element);


	// Popup
	void OpenPopup(UIPopup* popup);
	void ClosePopup(UIPopup* popup);


	// Implements IUIInjectedInputReceiver
	//virtual bool InjectViewportSizeChanged(int width, int height) override;
	virtual bool InjectMouseMove(float clientX, float clientY) override;
	virtual bool InjectMouseButtonDown(MouseButtons button, float clientX, float clientY) override;
	virtual bool InjectMouseButtonUp(MouseButtons button, float clientX, float clientY) override;
	virtual bool InjectMouseWheel(int delta) override;
	virtual bool InjectKeyDown(Keys keyCode, ModifierKeys modifierKeys) override;
	virtual bool InjectKeyUp(Keys keyCode, ModifierKeys modifierKeys) override;
	virtual bool InjectTextInput(TCHAR ch) override;

LN_CONSTRUCT_ACCESS:
	UILayoutView();
	virtual ~UILayoutView();
	void initialize(UIContext* ownerContext, PlatformWindow* ownerNativeWindow);

LN_INTERNAL_ACCESS:
	virtual detail::SpcialUIElementType GetSpcialUIElementType() const;

private:
	friend class UIContext;

	struct MouseClickTracker
	{
		double		LastTime = 0.0;
		int			ClickCount = 0;
		UIElement*	HoverElement = nullptr;
	};


	PlatformWindow*		m_ownerNativeWindow;

	UIContext*			m_ownerContext;
	UIElement*			m_mouseHoverElement;

	PointF				m_mousePosition;
	MouseClickTracker	m_mouseClickTrackers[8];

	Size				m_viewPixelSize;

	List<RefPtr<detail::UIPopuoContainer>>	m_popupContainers;
};

LN_NAMESPACE_END
