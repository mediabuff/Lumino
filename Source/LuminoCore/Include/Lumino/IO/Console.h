
#pragma once
#include "../Base/String.h"

LN_NAMESPACE_BEGIN

#ifdef LN_USTRING

class Console
{
public:

	static void writeLine();
	static void writeLine(const String& str);

};

#else
/**
	@brief		コンソールウィンドウの操作を行います。
*/
class Console
{
public:
	
	/**
		@brief		コンソールウィンドウを割り当てます。
	*/
	static void alloc();
	
	/**
		@brief		コンソールウィンドウを解放します。
	*/
	static void Free();

	static void writeLine();


	static void writeLine(const StringRefA& str);
	static void writeLine(const StringRefW& str);

	template<typename... TArgs>
	static void writeLine(const StringRefA& str, const TArgs&... args);
	template<typename... TArgs>
	static void writeLine(const StringRefW& str, const TArgs&... args);



	static void writeLineError();

	template<typename... TArgs>
	static void writeLineError(const StringRefA& str, const TArgs&... args);
	template<typename... TArgs>
	static void writeLineError(const StringRefW& str, const TArgs&... args);

private:
	static void writeInternal(const char* str);
	static void writeInternal(const wchar_t* str);
	static void writeInternalError(const char* str);
	static void writeInternalError(const wchar_t* str);
};

//------------------------------------------------------------------------------
template<typename... TArgs>
void Console::writeLine(const StringRefA& str, const TArgs&... args)
{
	StringA s = StringA::format(str, args...);
	writeInternal(s.c_str());
	writeLine();
}

//------------------------------------------------------------------------------
template<typename... TArgs>
void Console::writeLine(const StringRefW& str, const TArgs&... args)
{
	StringW s = StringW::format(str, args...);
	writeInternal(s.c_str());
	writeLine();
}

//------------------------------------------------------------------------------
template<typename... TArgs>
void Console::writeLineError(const StringRefA& str, const TArgs&... args)
{
	StringA s = StringA::format(str, args...);
	writeInternalError(s.c_str());
	writeLineError();
}

//------------------------------------------------------------------------------
template<typename... TArgs>
void Console::writeLineError(const StringRefW& str, const TArgs&... args)
{
	StringW s = StringW::format(str, args...);
	writeInternalError(s.c_str());
	writeLineError();
}
#endif

LN_NAMESPACE_END
