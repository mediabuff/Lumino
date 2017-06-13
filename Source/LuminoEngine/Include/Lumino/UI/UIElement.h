﻿
#pragma once
#include "../Base/GeometryStructs.h"
#include "../Graphics/Material.h"
#include "Common.h"
#include "Detail.h"
#include "UITypeInfo.h"
#include "UIEvent.h"
#include "LayoutElement.h"

LN_NAMESPACE_BEGIN
class DrawingContext;
class UIStylePropertyTable;
class UIVisualStateManager;
namespace detail { class UIStylePropertyTableInstance; }

enum class UISpecialElementType
{
	None,
	FrameWindow,
};

/**
	@brief		特定のイベントデータを持たない、UIイベントを処理するハンドラです。
	@param[in]	e		: イベントのデータ
*/
LN_DELEGATE()
using UIEventHandler = Delegate<void(UIEventArgs* e)>;

/**
	@brief		特定のイベントデータを持たない、UIイベントを処理するハンドラです。
	@param[in]	e		: イベントのデータ
*/
LN_DELEGATE()
using UIMouseEventHandler = Delegate<void(UIMouseEventArgs* e)>;

class UIVisualStateManager
	: public Object
{
private:
	struct Group
	{
		int				index;
		String			name;
		List<String>	stateNames;
	};

	List<Group>		groups;
	List<String>	activeStateNames;

public:
	void RegisterVisualState(const StringRef& groupName, const StringRef& stateName)
	{
		Group* group = groups.find([groupName](const Group& g) { return g.name == groupName; });
		if (group == nullptr)
		{
			groups.add(Group{ groups.getCount(), groupName });
			activeStateNames.add(stateName);
			group = &groups.getLast();
		}
		group->stateNames.add(stateName);
	}

	void GoToVisualState(const StringRef& stateName)
	{
		Group* group = FindGroup(stateName);//groups.Find([stateName](const Group& g) { return g.stateNames.Contains([stateName](const String& name) { return name == stateName; }); });
		if (LN_CHECK_STATE(group != nullptr)) return;
		activeStateNames[group->index] = stateName;
	}

LN_CONSTRUCT_ACCESS:
	void initialize()
	{
	}

	Group* FindGroup(const StringRef& stateName)
	{
		for (auto& g : groups)
		{
			for (auto& s : g.stateNames)
			{
				if (s == stateName)
					return &g;
			}
		}
		return nullptr;
	}

LN_INTERNAL_ACCESS:
	const List<String>& GetActiveStateNames() const { return activeStateNames; }
};

class UIVisualStates
{
public:
	static const String CommonGroup;
	static const String FocusGroup;
	//static const String ValidationStates;

	static const String NormalState;
	static const String MouseOverState;
	static const String PressedState;
	static const String DisabledState;
	static const String UnfocusedState;
	static const String FocusedState;
	static const String ValidState;
	static const String InvalidState;


	static const String OrientationGroup;
	static const String HorizontalState;
	static const String VerticalState;
};

/**
	@brief		
	@details	
*/
class UIElement
	: public RuntimeResource
	, public ILayoutElement
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();

public:
	LN_TR_PROPERTY(PointF,				position);				/**< Position プロパティの識別子 */
	LN_TR_PROPERTY(float,				width);
	LN_TR_PROPERTY(float,				height);
	LN_TR_PROPERTY(ThicknessF,			margin);
	LN_TR_PROPERTY(ThicknessF,			padding);
	LN_TR_PROPERTY(AlignmentAnchor,		anchor);				/**< Anchor プロパティの識別子 */
	LN_TR_PROPERTY(HAlignment,			hAlignment);			/**< HAlignment プロパティの識別子 */
	LN_TR_PROPERTY(VAlignment,			vAlignment);			/**< VAlignment プロパティの識別子 */
	//LN_TR_PROPERTY(BrushPtr,			background);			/**< Background プロパティの識別子 */
	LN_TR_PROPERTY(BrushPtr,			foreground);			/**< Foreground プロパティの識別子 */
	LN_TR_PROPERTY(BrushPtr,			decoratorBackground);	/**< DecoratorBackground プロパティの識別子 */
	LN_TR_PROPERTY(float,				decoratorOpacity);		/**< Foreground プロパティの識別子 */
	tr::Property<PointF>				position;
	tr::Property<float>					width;
	tr::Property<float>					height;
	tr::Property<ThicknessF>			margin;
	tr::Property<ThicknessF>			padding;
	tr::Property<AlignmentAnchor>		anchor;

	tr::Property<HAlignment>			hAlignment;
	tr::Property<VAlignment>			vAlignment;
	//tr::Property<BrushPtr>				background;
	tr::Property<BrushPtr>				foreground;
	tr::Property<BrushPtr>				decoratorBackground;
	tr::Property<float>					decoratorOpacity;

	// Border
	//tr::Property<ThicknessF>			borderThickness;
	//tr::Property<CornerRadius>			cornerRadius;
	//tr::Property<Color>					leftBorderColor;
	//tr::Property<Color>					topBorderColor;
	//tr::Property<Color>					rightBorderColor;
	//tr::Property<Color>					bottomBorderColor;
	//tr::Property<BorderDirection>		borderDirection;

	LN_ROUTED_EVENT(UIEventArgs, GotFocusEvent);
	LN_ROUTED_EVENT(UIEventArgs, LostFocusEvent);

public:
	//--------------------------------------------------------------------------
	/** @name RoutedEvents */
	/** @{ */

	UIEvent<UIMouseEventArgs>		MouseEnter;		/**< マウスポインタがこの要素の境界内に入ったときに発生します。*/
	UIEvent<UIMouseEventArgs>		MouseLeave;		/**< マウスポインタがこの要素の境界から出たときに発生します。*/
	UIEvent<UIMouseEventArgs>		MouseMove;		/**< マウスポインタがこの要素上で移動すると発生します。 */
	UIEvent<UIMouseEventArgs>		MouseDown;		/**< マウスポインタがこの要素上にあるときに任意のマウスボタンが押されると発生します。*/
	UIEvent<UIMouseEventArgs>		MouseUp;		/**< マウスポインタがこの要素上にあるときに任意のマウスボタンが離されると発生します。*/
	UIEvent<UIKeyEventArgs>			KeyDown;		/**< この要素がフォーカスを持っている時にキーボードのキーが押されると発生します。*/
	UIEvent<UIKeyEventArgs>			KeyUp;			/**< この要素がフォーカスを持っている時にキーボードのキーが離されると発生します。*/
	UIEvent<UIKeyEventArgs>			TextInput;		/**< キー操作により文字が入力されると発生します。*/
	UIEvent<UIEventArgs>			GotFocus;
	UIEvent<UIEventArgs>			LostFocus;

	/** @} */

	//--------------------------------------------------------------------------
	/** @name Properties */
	/** @{ */

	void setPosition(const PointF& value) { tr::PropertyInfo::setPropertyValueDirect<PointF>(this, positionId, value); }
	const PointF& getPosition() const { return tr::PropertyInfo::getPropertyValueDirect<PointF>(this, positionId); }

	void SetWidth(float value) { tr::PropertyInfo::setPropertyValueDirect<float>(this, widthId, value); }
	float getWidth() const { return tr::PropertyInfo::getPropertyValueDirect<float>(this, widthId); }

	void SetHeight(float value) { tr::PropertyInfo::setPropertyValueDirect<float>(this, heightId, value); }
	float getHeight() const { return tr::PropertyInfo::getPropertyValueDirect<float>(this, heightId); }

	void setSize(const Size& value) { SetWidth(value.width); SetHeight(value.height); }
	Size getSize() const { return Size(width, height); }

	void SetMinWidth(float value) { m_minSize.width = value; }

	void SetMinHeight(float value) { m_minSize.height = value; }

	void SetMaxWidth(float value) { m_maxSize.width = value; }

	void SetMaxHeight(float value) { m_maxSize.height = value; }

	void SetAnchor(AlignmentAnchor value) { tr::PropertyInfo::setPropertyValueDirect<AlignmentAnchor>(this, anchorId, value); }
	AlignmentAnchor GetAnchor() const { return tr::PropertyInfo::getPropertyValueDirect<AlignmentAnchor>(this, anchorId); }

	void SetHAlignment(HAlignment value) { tr::PropertyInfo::setPropertyValueDirect<HAlignment>(this, hAlignmentId, value); }
	HAlignment GetHAlignment() const { return tr::PropertyInfo::getPropertyValueDirect<HAlignment>(this, hAlignmentId); }

	void SetVAlignment(VAlignment value) { tr::PropertyInfo::setPropertyValueDirect<VAlignment>(this, vAlignmentId, value); }
	VAlignment GetVAlignment() const { return tr::PropertyInfo::getPropertyValueDirect<VAlignment>(this, vAlignmentId); }

	void SetBackground(Brush* value);
	Brush* GetBackground() const;


	void setOpacity(float value) { m_builtinEffectData.setOpacity(value); }
	float getOpacity() const { return m_builtinEffectData.getOpacity(); }

	/** @} */


public:


	/** 要素の識別名を取得します。*/
	const String& GetKeyName() const { return m_keyName; }

	/** 論理上の親要素を取得します。*/
	UIElement* GetLogicalParent() const { return m_logicalParent; }

	//void setSize(const Size& size) { m_size = size; }

	void SetFocusable(bool value) { m_isFocusable = value; }

	/** この要素がフォーカスを得ることができるかを確認します。*/
	bool IsFocusable() const { return m_isFocusable; }

	void SetHitTestVisible(bool value) { m_isHitTestVisible = value; }

	bool IsHitTestVisible() const { return m_isHitTestVisible; }

	/** レイアウト処理の測定パスの実行中にこの要素が計算したサイズを取得します。この値は子要素が親要素へ要求する、子要素自身の最低サイズです。*/
	const Size& getDesiredSize() const { return m_desiredSize; }

	/** この要素の最終的な描画サイズを取得します。この値は Arrange() で確定します。*/
	const Size& getRenderSize() const { return m_finalLocalRect.getSize(); }

	/** この要素へのフォーカスの取得を試みます。*/
	void Focus();

	/** マウスキャプチャを設定します。*/
	void CaptureMouse();

	/** マウスキャプチャを解除します。*/
	void ReleaseMouseCapture();

	/** この要素内の子ビジュアル要素の数を取得します。(論理要素も含めたすべての子要素) */
	virtual int getVisualChildrenCount() const override;

	/** Zオーダーやアクティブ状態を考慮した順で、子ビジュアル要素を取得します。奥にある要素が先、手前にある要素が後になります。*/
	virtual ILayoutElement* getVisualChild(int index) const override;

	/** この要素が関連付けられている UILayoutView を取得します。*/
	//UILayoutView* GetOwnerLayoutView() const { return m_ownerLayoutView; }


	virtual void measureLayout(const Size& availableSize) override;
	virtual void arrangeLayout(const Rect& finalLocalRect) override;

	// 登録されているハンドラと、(Bubbleの場合)論理上の親へイベントを通知する
	void RaiseEvent(const UIEventInfo* ev, UIElement* sender, UIEventArgs* e);

	void ApplyTemplateHierarchy(UIStyleTable* styleTable, detail::UIStylePropertyTableInstance* parentStyle);

	float GetActualWidth() const { return m_finalLocalRect.width; }
	float GetActualHeight() const { return m_finalLocalRect.height; }

	bool HasFocus() const { return m_hasFocus; }

	//--------------------------------------------------------------------------
	/** @name Grid layout */
	/** @{ */

	void SetLayoutColumn(int index);
	virtual int getLayoutColumn() const override;
	void SetLayoutRow(int index);
	virtual int getLayoutRow() const override;
	void SetLayoutColumnSpan(int span);
	virtual int getLayoutColumnSpan() const override;
	void SetLayoutRowSpan(int span);
	virtual int getLayoutRowSpan() const override;

	/** @} */

	
	/** OnGotFocus イベントの通知を受け取るコールバックを登録します。*/
	LN_METHOD(Event)
	EventConnection ConnectOnGotFocus(UIEventHandler handler);
	
	/** OnLostFocus イベントの通知を受け取るコールバックを登録します。*/
	LN_METHOD(Event)
	EventConnection ConnectOnLostFocus(UIEventHandler handler);

protected:
	UIElement();
	virtual ~UIElement();
	void initialize();

	/** 要素の視覚状態を切り替えます。*/
	void GoToVisualState(const StringRef& stateName);

	/** フォントによるレイアウト情報を無効化します。次のレイアウト更新パスで、フォント情報が再構築されます。*/
	void InvalidateFont() { m_invalidateFlags |= detail::InvalidateFlags::Font; }
	
	/**
		@brief		この要素を表示するために必要なサイズを計測します。
		@params[in]	constraint	: この要素を配置できる領域の最大サイズ。通常は親要素のサイズが渡されます。
		@return		この要素のレイアウトの際に必要となる最低限のサイズ。
					この要素のサイズと、全ての子要素のサイズに基づき決定します。NaN や Inf であってはなりません。
		@details	constraint は、ScrollViewer 等のコンテンツとなった場合は Infinity が渡されることがあります。
	*/
	virtual Size measureOverride(const Size& constraint) override;

	/**
		@brief		Visual 子要素の配置を確定し、この要素の最終サイズを返します。
		@param[in]	finalSize	: 親要素がこの要素に対して割り当てた領域のサイズ。
		@return		要素の最終サイズ。要素の描画時にこのサイズを使用します。
		@details	派生クラスは finalSize よりも大きいサイズを返すと、描画時に見切れが発生します。
					また、finalSize には padding プロパティの余白は考慮されません。
					この余白を正しく反映するためには派生クラスで padding プロパティを参照し、子要素の位置を計算します。

					親要素は、各子要素の Arrange を呼び出し、適切に配置する必要があります。
					そうでない場合、子要素はレンダリングされません。(UIElement::arrangeOverride() は、子要素の配置は行いません)
	*/
	virtual Size arrangeOverride(const Size& finalSize) override;

	virtual void OnUpdateFrame();

	/** この要素のレイアウトの更新が完了した時に呼び出されます。*/
	virtual void OnLayoutUpdated();

	/**
		@brief	この要素の描画を行います。
	*/
	virtual void onRender(DrawingContext* g);

	virtual void OnMouseMove(UIMouseEventArgs* e);
	virtual void OnMouseDown(UIMouseEventArgs* e);
	virtual void OnMouseUp(UIMouseEventArgs* e);
	virtual void OnMouseEnter(UIMouseEventArgs* e);
	virtual void OnMouseLeave(UIMouseEventArgs* e);
	virtual void OnKeyDown(UIKeyEventArgs* e);
	virtual void OnKeyUp(UIKeyEventArgs* e);
	virtual void OnTextInput(UIKeyEventArgs* e);
	virtual void OnGotFocus(UIEventArgs* e);
	virtual void OnLostFocus(UIEventArgs* e);

	virtual void OnUpdateStyle(detail::UIStylePropertyTableInstance* localStyle, detail::InvalidateFlags invalidateFlags);
	virtual void OnUpdatingLayout();

	//UIStylePropertyTable* GetLocalStyle() const { return m_localStyle; }

	virtual bool onEvent(detail::UIInternalEventType type, UIEventArgs* args);
	virtual void OnRoutedEvent(UIEventArgs* e);
	virtual void UpdateLayout(const Size& viewSize) override;
	virtual detail::SpcialUIElementType GetSpcialUIElementType() const;

LN_INTERNAL_ACCESS:
	detail::UIManager* getManager() const { return m_manager; }
	UIContext* GetContext() const;
	const PointF& GetPositionInternal() const { return position; }
	void SetSizeInternal(const Size& size) { width = size.width; height = size.height; }
	Size GetSizeInternal() const { return Size(width, height); }
	const ThicknessF& GetMargineInternal() const { return margin; }
	AlignmentAnchor GetAnchorInternal() const { return anchor; }
	const BrushPtr& GetForegroundInternal() const { return foreground; }
	void SetLogicalParent(UIElement* parent);
	UIVisualStateManager* GetVisualStateManager();
	void SetStyleSubControlName(const StringRef& ownerControlName, const StringRef& subControlName) { m_styleSubControlOwnerName = ownerControlName; m_styleSubControlName = subControlName; }
	//const String& GetStyleSubControlName() const { return m_styleSubControlName; }
	//const String& GetCurrentVisualStateName() const { return m_currentVisualStateName; }
	//AnchorInfo* GetAnchorInfo() {return &m_anchorInfo; }
	detail::InvalidateFlags GetInvalidateFlags() const { return m_invalidateFlags; }
	virtual UIElement* CheckMouseHoverElement(const PointF& globalPt);
	void CallOnGotFocus();
	void CallOnLostFocus();
	const Rect& GetFinalGlobalRect() const { return m_finalGlobalRect; }
	UIElement* GetVisualParent() const { return m_visualParent; }

	void SetSpecialElementType(UISpecialElementType type) { m_specialElementType = type; }
	UISpecialElementType GetSpecialElementType2() const { return m_specialElementType; }

	void updateFrame();
	void render(DrawingContext* g);

protected:
	virtual void updateTransformHierarchy(const Rect& parentGlobalRect) override;

LN_PROTECTED_INTERNAL_ACCESS:
	virtual const HAlignment* GetPriorityContentHAlignment();
	virtual const VAlignment* GetPriorityContentVAlignment();
	//virtual void GetStyleClassName(String* outSubStateName);

	/** 指定した要素をこの要素にビジュアル子要素として追加します。*/
	void AddVisualChild(UIElement* element);

	/** 指定した要素をこの要素のビジュアルツリーから削除します。*/
	void RemoveVisualChild(UIElement* element);

private:
	// ILayoutElement interface
	virtual const PointF& getLayoutPosition() const override;
	virtual Size getLayoutSize() const override;
	virtual const ThicknessF& getLayoutMargin() const override;
	virtual const ThicknessF& getLayoutPadding() const override;
	virtual AlignmentAnchor getLayoutAnchor() const override;
	virtual HAlignment getLayoutHAlignment() const override;
	virtual VAlignment getLayoutVAlignment() const override;
	virtual void getLayoutMinMaxInfo(Size* outMin, Size* outMax) const override;
	virtual ILayoutElement* getLayoutParent() const override;
	virtual const HAlignment* getLayoutContentHAlignment() override;
	virtual const VAlignment* getLayoutContentVAlignment() override;
	virtual const Size& getLayoutDesiredSize() const override;
	virtual void setLayoutDesiredSize(const Size& size) override;
	virtual void setLayoutFinalLocalRect(const Rect& rect) override;
	virtual const Rect& getLayoutFinalLocalRect() const override;
	virtual void setLayoutFinalGlobalRect(const Rect& rect) override;


private:
	void UpdateLocalStyleAndApplyProperties(UIStyleTable* styleTable, detail::UIStylePropertyTableInstance* parentStyleInstance);

	// 登録されているハンドラと、(Bubbleの場合)論理上の親へイベントを通知する
	void RaiseEventInternal(UIEventArgs* e);

	detail::UIManager*		m_manager;
	//UILayoutView*			m_ownerLayoutView;
	String					m_keyName;
	Size					m_minSize;
	Size					m_maxSize;
	UIElement*				m_logicalParent;
	//UIStylePropertyTable*	m_localStyle;			// 内部的に使用されるスタイル。親や VisualState から取得したスタイルをマージしたもの。
	//	m_localStyle;
	RefPtr<detail::UIStylePropertyTableInstance>	m_localStyle;
	Size					m_desiredSize;			// measureLayout() で決定されるこのコントロールの要求サイズ
	Rect					m_finalLocalRect;		// 描画に使用する最終境界矩形 (グローバル座標系=RootFrame のローカル座標系)
	Rect					m_finalGlobalRect;
	String					m_elementName;				// 要素名 ("UITextBlock" など) TODO: いらないかも
	RefPtr<UIVisualStateManager>	m_visualStateManager;
	String							m_styleSubControlOwnerName;
	String							m_styleSubControlName;

	UIElement*                m_visualParent;
	std::shared_ptr<List<RefPtr<UIElement>>>    m_visualChildren;

	// Property
	//		これらには直接値を設定しないこと。Property::setValueDirect() を使う。
	//		これによって必要にアニメーションを止めたりできる。
	//tr::Property<PointF>	m_position;
	//tr::Property<Size>		m_size;
	//tr::Property<AlignmentAnchor>	m_anchor;
	//tr::Property<HAlignment>		m_horizontalAlignment;
	//tr::Property<VAlignment>		m_verticalAlignment;
	detail::GridLayoutInfo	m_gridLayoutInfo;

	//tr::Property<BrushPtr>			m_background;
	//tr::Property<BrushPtr>			m_foreground;
	//detail::BorderInfo				m_border;



	//float							m_opacity;
	//ToneF							m_tone;
	detail::BuiltinEffectData			m_builtinEffectData;
	detail::BuiltinEffectData			m_combinedBuiltinEffectData;

	//tr::Property<BrushPtr>				m_decoratorBackground;
	//tr::Property<float>					m_decoratorOpacity;

	UISpecialElementType			m_specialElementType;

	//RefPtr<Style>					m_style;
	float					m_combinedOpacity;
	//AnchorInfo				m_anchorInfo;
	detail::InvalidateFlags	m_invalidateFlags;
	bool					m_isEnabled;
	bool					m_isMouseOver;
	bool					m_isHitTestVisible;
	bool					m_isFocusable;
	bool					m_hasFocus;

	UIEventHandler::EventType	m_onGotFocus;
	UIEventHandler::EventType	m_onLostFocus;

	friend class UILayoutView;
	friend class UIPopup;
	friend class UIContext;
	friend class UIHelper;
};

LN_NAMESPACE_END
