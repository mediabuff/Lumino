﻿
#pragma once
#include "../../CoreObject.h"
#include "../../CoreObjectList.h"
#include "../UIElement.h"
#include "Panel.h"
#include "../ContentControl.h"
#include "../ItemsControl.h"
#include "../Decorator.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GUI_BEGIN
class ListBox;

/**
	@brief
*/
class ListBoxItem
	: public ItemsControlItem
{
	LN_CORE_OBJECT_TYPE_INFO_DECL();
	LN_UI_ELEMENT_SUBCLASS_DECL(ListBoxItem);

public:
	static ListBoxItem* Create(GUIManagerImpl* manager);

	ListBoxItem(GUIManagerImpl* manager);
	virtual ~ListBoxItem();

private:
	void Handler_MouseEnter(MouseEventArgs* e);
	void Handler_MouseLeave(MouseEventArgs* e);
};


/**
	@brief
*/
class ListBoxItemList	// TODO: いらないかも
	: public GenericCoreList<ListBoxItem*>
{
	LN_CORE_OBJECT_TYPE_INFO_DECL();
public:
	typedef GenericCoreList<ListBoxItem*>::value_type value_type;

public:
	ListBoxItemList(ListBox* owner) : m_owner(owner) {}
	virtual ~ListBoxItemList() {}

protected:
	virtual void InsertItem(int index, const value_type& item);
	virtual void ClearItems();
	virtual void RemoveItem(int index);
	virtual void SetItem(int index, const value_type& item);

private:
	ListBox*	m_owner;
};

/**
	@brief		ListBox コントロールの外観です。
*/
class ListBoxChrome
	: public Decorator
{
	LN_CORE_OBJECT_TYPE_INFO_DECL();
	LN_UI_ELEMENT_SUBCLASS_DECL(ListBoxChrome);
public:
	//static const String	IsMouseOverProperty;	///< IsMouseOver プロパティの識別子
	//static const String	FrameWidthProperty;		///< FrameWidth プロパティの識別子

public:
	static ListBoxChromePtr Create();

	ListBoxChrome(GUIManagerImpl* manager);
	virtual ~ListBoxChrome();

public:
	// Property
	//void SetMouseOver(bool value) { m_isMouseOver = value; }
	//bool IsMouseOver() const { return m_isMouseOver; }

	///// ボタンイメージの外枠の幅を設定します。
	//void SetFrameWidth(float width) { m_frameWidth = width; }

	///// ボタンイメージの外枠の幅を取得します。
	//float GetFrameWidth() const { return m_frameWidth; }

protected:
	virtual void OnApplyTemplate(CombinedLocalResource* localResource);
	virtual void OnRender(RenderingContext* painter);

private:
	RefPtr<TextureBrush>	m_frameBrush;
	RefPtr<TextureBrush>	m_bgBrush;
	float							m_frameWidth;
	bool							m_isMouseOver;
};



/**
	@brief		
*/
class ListBox
	: public ItemsControl
{
	LN_CORE_OBJECT_TYPE_INFO_DECL();
	LN_UI_ELEMENT_SUBCLASS_DECL(ListBox);
public:

	/**
		@brief	ListBox を作成します。	
	*/
	static ListBoxPtr Create();


public:

	/**
		@brief	指定した文字列をリストボックスの項目として追加します。	
	*/
	ListBoxItemPtr AddTextItem(const String& text);
	ListBoxItemPtr AddItem(const UIElement& item);

	

	/**
		@brief	指定した文字列をリストボックスの項目として追加します。	
	*/
	//void SetItemGroup(int index, const Variant& groupKey);



	// TODO: こやつらは protected にするべき
	ListBox(GUIManagerImpl* manager);
	virtual ~ListBox();

	//ListBoxItemList* GetListBoxItems() { return m_listBoxItems; }

	// ユーティリティ
	void InsertListBoxItem(int index, UIElement* element);

	//ItemsPanel
protected:
#if 0
	// 子要素を持つ要素の override
	virtual UIElement* CheckMouseHoverElement(const PointF& globalPt);
	virtual void ApplyTemplateHierarchy(CombinedLocalResource* parent);
	virtual void MeasureLayout(const SizeF& availableSize);
	virtual void ArrangeLayout(const RectF& finalLocalRect);
	virtual bool OnEvent(EventType type, RoutedEventArgs* args);
	virtual void Render();
#endif

	// ListBoxItemList callback
	friend class ListBoxItemList;
	void OnListBoxItemAdded(ListBoxItem* item);
	void OnListBoxItemRemoved(ListBoxItem* item);

	//virtual void Render();
	//virtual UIElement* CheckMouseHoverElement(const PointF& globalPt);
	//virtual bool OnEvent(EventType type, RoutedEventArgs* args);
	//virtual void ApplyTemplateHierarchy(CombinedLocalResource* parent);
	//virtual void ApplyTemplate(CombinedLocalResource* parent);
	//virtual void MeasureLayout(const SizeF& availableSize);
	//virtual void ArrangeLayout(const RectF& finalLocalRect);

protected:
	//RefPtr<ListBoxItemList>	m_listBoxItems;		///< logical children
	RefPtr<Panel>			m_itemsPanel;		///< ListBoxItem が配置される Panel


	// ListBox のスタイルとテンプレート
	// https://msdn.microsoft.com/ja-jp/library/ms754242%28v=vs.110%29.aspx
};


LN_NAMESPACE_GUI_END
LN_NAMESPACE_END
