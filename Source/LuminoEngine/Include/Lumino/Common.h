﻿
#pragma once
#include <Lumino/Base/Common.h>

#define LNR_CLASS()
#define LNR_METHOD()
#define LNR_PROPERTY()

LN_NAMESPACE_BEGIN

/** マウスボタン */
enum class MouseButtons
{
	None = 0,		/**< 無効または押されていないことを示す */
	Left,			/**< 左ボタン */
	Right,			/**< 右ボタン */
	Middle,			/**< 中央ボタン */

	X1,				/**< 拡張ボタン1 */
	X2,				/**< 拡張ボタン2 */
};

/** キーコード (NumPad はサポートしていない) */
enum class Keys
{
	Unknown = 0,

	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,

	D0,
	D1,
	D2,
	D3,
	D4,
	D5,
	D6,
	D7,
	D8,
	D9,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	Space,	// 0x20 = ' ' (スペース)
	Escape,
	Up,
	Down,
	Left,
	Right,
	LShift,
	RShift,
	LCtrl,  // TODO: LeftControl
	RCtrl,
	LAlt,       // mac:option
	RAlt,
	Tab,
	Enter,
	BackSpace,
	Insert,
	Delete,
	PageUp,
	PageDown,
	Home,
	End,

	Colon,		///< :	[非推奨 GLFWでは;]
	Semicolon,	///< ;	[非推奨 GLFWでは=]
	Comma,		///< ,
	Period,		///< .
	Slash,		///< /
	Minus,		///< -
	Backslash,	///< \(バックスラッシュ) [非推奨 GLFWではNOWORD2]
	Yen,		///< \(BaskSpaceキーの左の\キー) [非推奨 GLFWではI]
	Caret,		///< ^ [非推奨 GLFWではGLFW_KEY_APOSTROPHE(')]
	LBracket,	///< [
	RBracket,	///< ]

	AndroidPlus,		///< Android '+'

	TERMINATOR,
};

/** 修飾キー */
LN_ENUM_FLAGS(ModifierKeys)
{
	None	= 0x0000,
	Alt		= 0x0001,	/**< Alt キー */
	Shift	= 0x0002,	/**< Shift キー */
	Control	= 0x0004,	/**< Ctrl キー */
};
LN_ENUM_FLAGS_DECLARE(ModifierKeys);

/** フレーム更新モード */
enum class FrameUpdateMode
{
	/**
		@brief		固定フレームレート
		@details	ランタイムエンジンが扱う1フレームの経過時間が、常に一定となります。(60FPS であれば 0.0166)
	*/
	Fixed,

	/**
		@brief		可変フレームレート (GameTime)
		@details	ランタイムエンジンが扱う1フレームの経過時間が、実際の経過時間となります。
					ただし、アプリケーションが非アクティブとなっている間や、Engine::resetFrameDelay() で遅延をリセットした分は含まれません。
					つまり、実際にゲームが動作している時間のみを対象とした可変フレームレートモードです。
	*/
	VariableOnGameTime,

	/**
		@brief		可変フレームレート (RealTime)
		@details	ランタイムエンジンが扱う1フレームの経過時間が、実際の経過時間となります。
					この経過時間は Engine::resetFrameDelay() などの特殊な時間を考慮しない、現実の経過時間です。
	*/
	VariableOnRealTime,
};

/**
	@brief	オブジェクトに関連付けられたリソースを解放するためのメカニズムを提供します。
*/
LN_CLASS()
class RuntimeResource
	: public Object
{
	LN_OBJECT;
protected:
	RuntimeResource();
	virtual ~RuntimeResource();
	virtual void finalize_() override;

	/** リソースの解放を行います。 */
	virtual void dispose();
};

LN_NAMESPACE_END
