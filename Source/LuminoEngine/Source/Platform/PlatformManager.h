﻿
#pragma once
#include <Lumino/Base/RefObject.h>
#include <Lumino/Base/String.h>
#include <Lumino/Base/List.h>
#include <Lumino/Threading/Thread.h>
#include <Lumino/Platform/Common.h>
#include <Lumino/Base/GeometryStructs.h>

LN_NAMESPACE_BEGIN

/// ウィンドウを作成するための設定
struct WindowCreationSettings
{
	String		title;							// ウィンドウタイトル
	SizeI		clientSize = SizeI(640, 480);	// クライアント領域のピクセルサイズ
	bool		fullscreen = false;				// フルスクリーンモードで作成するかどうか
	bool		resizable = true;				// 可変ウィンドウとして作成するかどうか
	intptr_t	userWindow = 0;
};


class PlatformManager
	: public RefObject
{
public:
	/// Application を初期化するための設定
	struct Settings
	{
		WindowSystemAPI			windowSystemAPI = WindowSystemAPI::Default;
		WindowCreationSettings	mainWindowSettings;
		bool					useInternalUIThread = false;

		//void*					ExternalMainWindow;		/**< ユーザー定義のウィンドウハンドル (windows の場合は HWND、X11 は Window*。ただし、X11 は未対応) */
	};

	static PlatformManager* getInstance(PlatformManager* priority = nullptr);

public:
	PlatformManager();
	PlatformManager(const Settings& settings);
	virtual ~PlatformManager();

public:

	void initialize(const Settings& settings);
	PlatformWindow* getMainWindow();
	bool doEvents();

	WindowManagerBase* getWindowManager() const LN_NOEXCEPT { return m_windowManager; }

public:
	void dispose();

protected:

private:
	void thread_MainWindow();

private:
	bool						m_useThread;
	WindowCreationSettings		m_windowCreationSettings;
	WindowManagerBase*			m_windowManager;
	DelegateThread				m_mainWindowThread;
	ConditionFlag				m_mainWindowThreadInitFinished;
	ConditionFlag				m_mainWindowThreadEndRequested;
};

LN_NAMESPACE_END
