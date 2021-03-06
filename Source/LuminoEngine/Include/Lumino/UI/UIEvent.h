﻿
#pragma once
#include <functional>
#include <Lumino/Reflection/Notify.h>
#include <Lumino/Reflection/TypeInfo.h>
#include "Common.h"
#include "UIEventArgs.h"

LN_NAMESPACE_BEGIN

///**
//	@brief		
//*/
//class UIEventSlotBase
//{
//protected:
//	UIEventSlotBase() = default;
//	virtual ~UIEventSlotBase() = default;
//
//private:
//	friend class UIElement;
//	virtual void Raise(UIEventArgs* e) = 0;
//};
/**
	@brief		
	@details
*/
template<class TArgs>
class UIEvent
	: public tr::ReflectionEvent<TArgs>
{
public:
	UIEvent() = default;
	virtual ~UIEvent() = default;
};
///**
//	@brief		
//	@details	RoutedEvent は UIElement 及びそのサブクラス内部からのみ発生させることが出来ます。
//*/
//template<class TArgs>
//class UIEventSlot
//	: public UIEventSlotBase
//{
//public:
//	UIEventSlot() = default;
//	virtual ~UIEventSlot() = default;
//
//public:
//
//	/**
//		@brief	イベントのハンドラを追加します。
//	*/
//	void addHandler(const Delegate<void(TArgs*)>& handler)
//	{
//		m_handlerList.Add(handler);
//	}
//	
//	/**
//		@brief	指定したハンドラに一致するハンドラを、このスロットから削除します。
//	*/
//	void removeHandler(const Delegate<void(TArgs*)>& handler)
//	{
//		m_handlerList.Remove(handler);
//	}
//	
//	/**
//		@brief	イベントのハンドラを追加します。
//	*/
//	void operator += (const std::function<void(TArgs*)>& handler)
//	{
//		addHandler(handler);
//	}
//
//	void operator += (const Delegate<void(TArgs*)>& handler)
//	{
//		m_handlerList.Add(handler);
//	}
//	
//	/**
//		@brief	指定したハンドラに一致するハンドラを、このスロットから削除します。
//	*/
//	void operator -= (const Delegate<void(TArgs*)>& handler)
//	{
//		m_handlerList.Remove(handler);
//	}
//
//private:
//	Array< Delegate<void(TArgs*)> > m_handlerList;
//
//	virtual void Raise(UIEventArgs* e)
//	{
//		for (Delegate<void(TArgs*)>& d : m_handlerList)
//		{
//			d.Call(static_cast<TArgs*>(e));
//		}
//	}
//};
//
/**
	@brief		
*/
//class UIEventInfo
//	: public tr::ReflectionEventInfo
//{
//public:
//	UIEventInfo(tr::TypeInfo* ownerClass, const Char* name, RaiseEventFunc raiseEvent)
//		: tr::ReflectionEventInfo(ownerClass, name, raiseEvent)
//	{}
//};
//	// Event を直接参照してはならない。このクラスは Property と同じく、複数の UIElement で共有される。状態を持ってはならない。
//	// なので、イベントを Raise する関数ポインタを参照する。
//
//	typedef void(*RaiseEventFunc)(UIElement* obj, UIEventArgs* e);
//
//public:
//	UIEvent(UITypeInfo* ownerClass, const String& name, RaiseEventFunc raiseEvent);
//	~UIEvent();
//
//public:
//	//const String& getName() const { return m_name; }
//
//	//void callEvent(UIElement* target, UIEventArgs* e) const
//	//{
//	//	m_raiseEvent(target, e);
//	//}
//
//private:
//	//friend class UITypeInfo;
//	//String		m_name;
//	//RaiseEventFunc	m_raiseEvent;
//	//bool	m_registerd;
//};
//
//#define LN_ROUTED_EVENT(eventArgs, eventVar) \
//	public:  static const UIEvent* eventVar; \
//	private: static void	_raise_##eventVar(UIElement* obj, UIEventArgs* e); \
//	private: static UIEvent _init_##eventVar;
//
//#define LN_ROUTED_EVENT_IMPLEMENT(ownerClass, eventArgs, eventVar, name, slot) \
//	UIEvent					ownerClass::_init_##eventVar(static_cast<UITypeInfo*>(tr::TypeInfo::getTypeInfo<ownerClass>()), name, &ownerClass::_raise_##eventVar); \
//	const UIEvent*			ownerClass::eventVar = &_init_##eventVar; \
//	void					ownerClass::_raise_##eventVar(UIElement* obj, UIEventArgs* e) { static_cast<ownerClass*>(obj)->EmitEventSlot(static_cast<ownerClass*>(obj)->slot, static_cast<eventArgs*>(e)); }
//
//
//using UIEventId = UIEventInfo*;
//
//// [Obsolete]
//#define LN_ROUTED_EVENT(eventArgs, eventInfoVar) \
//	LN_REFLECTION_EVENT_COMMON(UIEventInfo, eventArgs, eventInfoVar);
//
//// [Obsolete]
//#define LN_ROUTED_EVENT_IMPLEMENT(ownerClass, eventArgs, eventInfoVar, name, ev) \
//	LN_REFLECTION_EVENT_IMPLEMENT_COMMON(UIEventInfo, ownerClass, eventArgs, eventInfoVar, name, ev);
//
//#define LN_ROUTED_EVENT2(eventArgs, eventVar) \
//    static const UIEventId eventVar##Id;
//
//#define LN_ROUTED_EVENT_IMPLEMENT2(ownerClass, eventArgs, eventVar) \
//    const UIEventId ownerClass::eventVar##Id = reinterpret_cast<UIEventId>(ln::Hash::calcHash(#eventVar));


/** */
class UIEventManager
{
public:
	static UIEventType RegisterEvent();
};

/** */
class UIEvents
{
public:
	/** GotFocusEvent イベントの識別子 (UIEventArgs) */
	static UIEventType	GotFocusEvent;

	/** LostFocusEvent イベントの識別子 (UIEventArgs) */
	static UIEventType	LostFocusEvent;

	/** MouseMove イベントの識別子 (UIMouseEventArgs) */
	static UIEventType	MouseMoveEvent;

	/** MouseDown イベントの識別子 (UIMouseEventArgs) */
	static UIEventType	MouseDownEvent;

	/** MouseUp イベントの識別子 (UIMouseEventArgs) */
	static UIEventType	MouseUpEvent;

	/** MouseWheel イベントの識別子 (UIMouseWheelEventArgs) */
	static UIEventType	MouseWheelEvent;

	/** MouseEnterEvent イベントの識別子 (UIMouseEventArgs) */
	static UIEventType	MouseEnterEvent;

	/** MouseLeaveEvent イベントの識別子 (UIMouseEventArgs) */
	static UIEventType	MouseLeaveEvent;

	/** KeyDownEvent イベントの識別子 (UIKeyEventArgs) */
	static UIEventType	KeyDownEvent;

	/** KeyUpEvent イベントの識別子 (UIKeyEventArgs) */
	static UIEventType	KeyUpEvent;

	/** TextInputEvent イベントの識別子 (UIKeyEventArgs) */
	static UIEventType	TextInputEvent;

	/** Checked イベントの識別子 (UIEventArgs) */
	static UIEventType	CheckedEvent;

	/** Unchecked イベントの識別子 (UIEventArgs) */
	static UIEventType	UncheckedEvent;

	/** DragStartedEvent イベントの識別子 (UIEventArgs) マウスドラッグを開始したときに発生するイベントを表します。*/
	static UIEventType	DragStartedEvent;

	/** DragDeltaEvent イベントの識別子 (UIEventArgs) マウスドラッグ中に発生するイベントを表します。 */
	static UIEventType	DragDeltaEvent;

	/** DragCompletedEvent イベントの識別子 (UIEventArgs) マウスドラッグが完了したときに発生するイベントを表します。 */
	static UIEventType	DragCompletedEvent;

	/** DragCanceledEvent イベントの識別子 (UIEventArgs) マウスドラッグを中断したときに発生するイベントを表します。 */
	static UIEventType	DragCanceledEvent;

	/** ScrollEvent イベントの識別子 (UIEventArgs) */
	static UIEventType	ScrollEvent;
};

/**
	@brief		特定のイベントデータを持たない、UIイベントを処理するハンドラです。
	@param[in]	e		: イベントのデータ
*/
LN_DELEGATE()
using UIEventHandler = Delegate<void(UIEventArgs* e)>;

/**
	@brief		マウス操作が関係する UIイベントを処理するハンドラです。
	@param[in]	e		: イベントのデータ
*/
LN_DELEGATE()
using UIMouseEventHandler = Delegate<void(UIMouseEventArgs* e)>;



LN_NAMESPACE_END
