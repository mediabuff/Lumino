
#include "Internal.h"
#include <Lumino/UI/UITextBlock.h>
#include <Lumino/UI/UIScrollViewer.h>
#include <Lumino/UI/UIComboBox.h>
#include <Lumino/UI/UILayoutPanel.h>	// TODO: stackpanel にするかも
#include <Lumino/UI/UILayoutView.h>
#include "UIManager.h"
#include "UIHelper.h"

LN_NAMESPACE_BEGIN
namespace tr
{

//==============================================================================
// UIPopup
//==============================================================================
LN_UI_TYPEINFO_IMPLEMENT(UIPopup, UIContentControl)

//------------------------------------------------------------------------------
UIPopup::UIPopup()
{
}

//------------------------------------------------------------------------------
UIPopup::~UIPopup()
{
}

//------------------------------------------------------------------------------
void UIPopup::Initialize()
{
	UIElement::Initialize(ln::detail::EngineDomain::GetUIManager());
}

//------------------------------------------------------------------------------
void UIPopup::SetContent(UIElement* element)
{
	if (m_content == element) return;

	m_content = element;

	// 既に持っていれば取り除いておく
	if (m_content != nullptr)
	{
		RemoveVisualChild(m_content);
		m_content = nullptr;
	}

	// 新しく保持する
	if (element != nullptr)
	{
		AddVisualChild(element);
		m_content = element;
	}
}

//------------------------------------------------------------------------------
void UIPopup::Open()
{
	UIElement* root = UIHelper::GetLayoutRoot(this);
	if (root != nullptr)
	{
		UILayoutView* rootView = static_cast<UILayoutView*>(root);
		rootView->OpenPopup(this);
	}
}

//------------------------------------------------------------------------------
Size UIPopup::MeasureOverride(const Size& constraint)
{
	// Popup は常にサイズ 0 となる。
	// また、子要素のレイアウトは行わない。
	// 子要素のレイアウトを行うのは別途、Popup 専用のレイアウトフェーズ。
	return Size();
}

//------------------------------------------------------------------------------
Size UIPopup::ArrangeOverride(const Size& finalSize)
{
	return UIElement::ArrangeOverride(finalSize);
}

//------------------------------------------------------------------------------
void UIPopup::UpdateLayoutForInPlacePopup(const Size& viewSize)
{
	m_content->UpdateLayout(viewSize);
	//m_content->MeasureLayout(viewSize);

	////TODO: このへんでchildの位置を決める

	//m_content->ArrangeLayout(RectF(0, 0, viewSize));
	//m_content->UpdateTransformHierarchy(RectF(0, 0, viewSize));
}


//==============================================================================
// UIComboBoxItem
//==============================================================================
LN_UI_TYPEINFO_IMPLEMENT(UIComboBoxItem, UIContentControl)

//------------------------------------------------------------------------------
UIComboBoxItem::UIComboBoxItem()
{
}

//------------------------------------------------------------------------------
UIComboBoxItem::~UIComboBoxItem()
{
}

//------------------------------------------------------------------------------
void UIComboBoxItem::Initialize(ln::detail::UIManager* manager)
{
	UIContentControl::Initialize(manager);
	SetHContentAlignment(HAlignment::Left);
	SetHAlignment(HAlignment::Stretch);
	GoToVisualState(NormalState);
}

//==============================================================================
// UIComboBox
//==============================================================================
LN_UI_TYPEINFO_IMPLEMENT(UIComboBox, UIControl)


//------------------------------------------------------------------------------
UIComboBoxPtr UIComboBox::Create()
{
	auto ptr = UIComboBoxPtr::MakeRef();
	ptr->Initialize(ln::detail::UIManager::GetInstance());
	return ptr;
}

//------------------------------------------------------------------------------
UIComboBox::UIComboBox()
{
}

//------------------------------------------------------------------------------
UIComboBox::~UIComboBox()
{
}

//------------------------------------------------------------------------------
void UIComboBox::Initialize(ln::detail::UIManager* manager)
{
	UIControl::Initialize();

	m_popup = NewObject<UIPopup>();
	AddVisualChild(m_popup);

	m_scrollViewer = NewObject<UIScrollViewer>();
	m_scrollViewer->SetWidth(100);	// TODO:
	m_scrollViewer->SetHeight(30);
	m_scrollViewer->SetBackground(Brush::Blue);
	m_popup->SetContent(m_scrollViewer);

	SetHContentAlignment(HAlignment::Stretch);

	auto panel = RefPtr<UIStackPanel>::MakeRef();
	panel->Initialize(manager);
	panel->SetHAlignment(HAlignment::Stretch);
	panel->SetVAlignment(VAlignment::Stretch);
	SetLayoutPanel(panel);
	GoToVisualState(NormalState);
}

//------------------------------------------------------------------------------
//UIComboBoxItemPtr UIComboBox::AddTextItem(const String& text)
//{
//	auto textBlock = RefPtr<UITextBlock>::MakeRef();
//	textBlock->Initialize(GetManager());
//	textBlock->SetText(text);
//	return AddItem(textBlock);
//}

//------------------------------------------------------------------------------
//UIComboBoxItemPtr UIComboBox::AddItem(UIElement* item)
//{
//	if (LN_CHECK_ARG(item != nullptr)) return nullptr;
//
//	// 受け取った item を UIComboBoxItem でラップして、UIComboBoxItem をリストに入れる
//	auto listItem = RefPtr<UIComboBoxItem>::MakeRef();
//	listItem->Initialize(GetManager());
//	listItem->SetContent(item);
//	GetItems()->Add(listItem);
//	return listItem;
//}

//------------------------------------------------------------------------------
void UIComboBox::OnMouseDown(UIMouseEventArgs* e)
{
	m_popup->Open();
	UIControl::OnMouseDown(e);
}

} // namespace tr
LN_NAMESPACE_END
