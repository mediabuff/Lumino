﻿
#pragma once
#include <Lumino/Base/List.h>
#include "PlatformEventArgs.h"

LN_NAMESPACE_BEGIN

/**
	@brief	
*/
class IEventListener
{
public:

	/**
		@brief	メッセージ処理コールバック
		@return true の場合、呼び出し側で他の処理を行わずに、すぐに処理を終了します。
	*/
	virtual bool onPlatformEvent(const PlatformEventArgs& e) = 0;

protected:
	virtual ~IEventListener() = default;
};

LN_NAMESPACE_END



