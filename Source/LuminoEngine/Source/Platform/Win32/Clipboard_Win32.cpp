﻿
#include "../Internal.h"
#include <Lumino/Text/Encoding.h>
#include <Lumino/Platform/Clipboard.h>
#include <Lumino/Platform/PlatformSupport.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// Clipboard
//==============================================================================

//------------------------------------------------------------------------------
void Clipboard::setText(PlatformWindow* window, const String& text)
{
	//ByteBuffer wideStr = text.convertTo(Encoding::getWideCharEncoding());
	//int wideCount = (wideStr.getSize() + 1) * sizeof(WCHAR);
	std::wstring wideStr = text.toStdWString();
	int byteCount = (wideStr.length() * 1) * sizeof(WCHAR);

	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, byteCount);
	if (LN_ENSURE_WIN32(hGlobal != NULL, ::GetLastError())) return;

	WCHAR* buf = (WCHAR*)::GlobalLock(hGlobal);
	memcpy(buf, wideStr.c_str(), byteCount);
	buf[wideStr.length()] = L'\0';
	::GlobalUnlock(hGlobal);

	HWND hWnd = PlatformSupport::getWindowHandle(window);
	BOOL r = ::OpenClipboard(hWnd);
	if (r == FALSE)
	{
		DWORD err = ::GetLastError();
		::GlobalFree(hGlobal);
		if (LN_ENSURE_WIN32(hGlobal != NULL, err)) return;
	}

	::EmptyClipboard();
	::SetClipboardData(CF_UNICODETEXT, buf);
	::CloseClipboard();
}

//------------------------------------------------------------------------------
String Clipboard::getText(PlatformWindow* window)
{
	// クリップボードのデータ形式チェック
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		return String::getEmpty();
	}

	HWND hWnd = PlatformSupport::getWindowHandle(window);
	BOOL r = ::OpenClipboard(hWnd);
	if (LN_ENSURE_WIN32(r != FALSE, ::GetLastError())) return String();

	HGLOBAL hGlobal = ::GetClipboardData(CF_UNICODETEXT);
	if (hGlobal == NULL)
	{
		DWORD err = ::GetLastError();
		::CloseClipboard();
		if (LN_ENSURE_WIN32(hGlobal != NULL, err)) return String();
	}

	WCHAR* buf = (WCHAR*)::GlobalLock(hGlobal);
	int len = wcslen(buf);

	String str;
	try
	{
		str = String::fromCString(buf, len);
	}
	catch (...)
	{
		::GlobalUnlock(hGlobal);
		::CloseClipboard();
		throw;
	}

	::GlobalUnlock(hGlobal);
	::CloseClipboard();
	return str;
}

LN_NAMESPACE_END
