﻿
#pragma once
#include <Lumino/Base/RefObject.h>
#include <Lumino/Base/Size.h>
#include <Lumino/Base/String.h>
#include <Lumino/Base/Array.h>
#include <Lumino/Threading/Thread.h>
#include <Lumino/Platform/Common.h>

LN_NAMESPACE_BEGIN

/// ウィンドウを作成するための設定
struct WindowCreationSettings
{
	String	title;							// ウィンドウタイトル
	Size	clientSize = Size(640, 480);	// クライアント領域のピクセルサイズ
	bool	fullscreen = false;				// フルスクリーンモードで作成するかどうか
	bool	resizable = true;				// 可変ウィンドウとして作成するかどうか
	void*	userWindow = nullptr;
};


class PlatformManager
	: public RefObject
{
public:
	/// Application を初期化するための設定
	struct Settings
	{
		WindowSystemAPI			API = WindowSystemAPI::Default;
		WindowCreationSettings	MainWindowSettings;
		bool					UseInternalUIThread = false;

		//void*					ExternalMainWindow;		/**< ユーザー定義のウィンドウハンドル (windows の場合は HWND、X11 は Window*。ただし、X11 は未対応) */
	};

public:
	PlatformManager();
	PlatformManager(const Settings& settings);
	virtual ~PlatformManager();

public:

	void Initialize(const Settings& settings);
	PlatformWindow* GetMainWindow();
	bool DoEvents();

public:
	void Dispose();

protected:

private:
	void Thread_MainWindow();

private:
	friend class PlatformWindow;
	bool						m_useThread;
	WindowCreationSettings		m_windowCreationSettings;
	WindowManagerBase*			m_windowManager;
	//PlatformWindow*						m_mainWindow;
	Threading::DelegateThread	m_mainWindowThread;
	Threading::EventFlag		m_mainWindowThreadInitFinished;
	Threading::EventFlag		m_mainWindowThreadEndRequested;
};

LN_NAMESPACE_END