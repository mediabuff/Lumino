
#pragma once
#include "UIControl.h"

LN_NAMESPACE_BEGIN

/**
	@brief		
*/
class UIRoutedCommand
	: public Object
{
	LN_UI_TYPEINFO_DECLARE();
public:
	static RefPtr<UIRoutedCommand> Create();

LN_CONSTRUCT_ACCESS:
	UIRoutedCommand();
	virtual ~UIRoutedCommand();
	void Initialize();

private:
};

class UIApplicationCommands
{
public:
	static UIRoutedCommand* Paste;
};

LN_NAMESPACE_END
