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
	ByteBuffer wideStr = text.convertTo(Encoding::getWideCharEncoding());
	int wideCount = (wideStr.getSize() + 1) * sizeof(WCHAR);

	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, wideCount);
	LN_THROW(hGlobal != NULL, Win32Exception, ::GetLastError());

	WCHAR* buf = (WCHAR*)::GlobalLock(hGlobal);
	memcpy(buf, wideStr.getConstData(), wideCount);
	buf[wideCount - 1] = L'\0';
	::GlobalUnlock(hGlobal);

	HWND hWnd = PlatformSupport::GetWindowHandle(window);
	BOOL r = ::OpenClipboard(hWnd);
	if (r == FALSE)
	{
		DWORD err = ::GetLastError();
		::GlobalFree(hGlobal);
		LN_THROW(hGlobal != NULL, Win32Exception, err);
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

	HWND hWnd = PlatformSupport::GetWindowHandle(window);
	BOOL r = ::OpenClipboard(hWnd);
	LN_THROW(r != FALSE, Win32Exception, ::GetLastError());

	HGLOBAL hGlobal = ::GetClipboardData(CF_UNICODETEXT);
	if (hGlobal == NULL)
	{
		DWORD err = ::GetLastError();
		::CloseClipboard();
		LN_THROW(hGlobal != NULL, Win32Exception, err);
	}

	WCHAR* buf = (WCHAR*)::GlobalLock(hGlobal);
	int len = wcslen(buf);

	String str;
	try
	{
		str.convertFrom(buf, len * sizeof(WCHAR), Encoding::getWideCharEncoding());
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
