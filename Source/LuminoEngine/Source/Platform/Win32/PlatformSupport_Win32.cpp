﻿
#include "../Internal.h"
#include <Lumino/Platform/PlatformSupport.h>
#include <Lumino/Platform/Win32/Win32PlatformWindow.h>
#include "../PlatformManager.h"

LN_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
void PlatformSupport::showAlertMessageBox(const Char* message) LN_NOEXCEPT
{
	HWND owner = NULL;
	PlatformManager* manager = PlatformManager::getInstance();
	if (manager != nullptr)
	{
		try
		{
			owner = getWindowHandle(manager->getMainWindow());
		}
		catch (...)
		{
		}
	}

	::MessageBox(owner, message, NULL, MB_ICONERROR);
}

//------------------------------------------------------------------------------
HWND PlatformSupport::getWindowHandle(PlatformWindow* window)
{
	Win32PlatformWindow* w = dynamic_cast<Win32PlatformWindow*>(window);
	if (LN_REQUIRE(w)) return NULL;
	return w->getWindowHandle();
}

LN_NAMESPACE_END
