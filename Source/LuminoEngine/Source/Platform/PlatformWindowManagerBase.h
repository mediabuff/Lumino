﻿
#pragma once
#include "PlatformManager.h"

LN_NAMESPACE_BEGIN

/**
	@note	[2015/2/8]
			なんやかんやで PlatformManager と WindowManager を分ける方向にした。
			
			PlatformManager に全部まとめるとすると、それを継承した Win32Manager とかを
			作ることになり、ウィンドウ作成やメッセージループはサブクラスで定義する。
			プラットフォームの違いをポリモーフィズムで表現するならこれが自然。

			ただ、この方法だとさらに上のレベルでプラットフォームを考慮した
			new をしなければならない。(これはファクトリ関数使えばまぁ何とかなるが)
			さらに、終了処理をデストラクタで行うことができない。
			「delete する前には必ず dispose() のような終了処理を呼んでくださいね」になる。

			特に、スレッド関数から仮想関数を呼び出している時、デストラクタでスレッド終了待ちなんてことをすると、
			pre call virtual function とか一見わけわからないエラーが発生する。

			LightNote → Lumino で各モジュールを細分化して公開することにした以上、
			PlatformManager は単体で使うことがある。(既に、現時点で仕事で使うあてがある)
			外部に公開する以上、可能な限りシンプルであるべき。
*/
class WindowManagerBase
	: public RefObject
{
protected:
	WindowManagerBase() { m_endRequested = false; }
	virtual ~WindowManagerBase() {}

public:
	virtual void createMainWindow(const WindowCreationSettings& settings) = 0;
	virtual PlatformWindow* getMainWindow() = 0;
	virtual PlatformWindow* createSubWindow(const WindowCreationSettings& settings) = 0;
	virtual void doEvents() = 0;
	virtual void dispose() = 0;

public:
	void addWindow(PlatformWindow* window) { m_windowArray.add(window); }
	void removeWindow(PlatformWindow* window) { m_windowArray.remove(window); }
	bool isEndRequested() const { return m_endRequested; }
	void exit() { m_endRequested = true; }

protected:
	List<PlatformWindow*>	m_windowArray;
	bool					m_endRequested;
};

LN_NAMESPACE_END

