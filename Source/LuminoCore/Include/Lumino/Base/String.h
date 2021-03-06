﻿
#pragma once
#include <vector>
#include <string>
#include <atomic>
#include <locale>
#include "Common.h"
#include "List.h"
#include "ByteBuffer.h"

#include "StringHelper.h"

#ifdef LN_UNICODE
#define LN_STRING_FROM_CHAR
#endif

LN_NAMESPACE_BEGIN
class Encoding;
namespace detail { class UStringCore; }

namespace detail
{

template<int N>
bool getLSB(unsigned char byte)
{
	return byte & (1u << N);
}

template<int N>
void setLSB(unsigned char& byte, bool bit)
{
	if (bit) {
		byte |= 1u << N;
	}
	else {
		byte &= ~(1u << N);
	}
}

enum class UStringRefSource
{
	ByUChar,
	ByChar,
};

}

class Locale;
class StringRef;
class Path;

/**
	@brief		文字列を表すクラスです。				
*/
class String
{
public:
	String();
	~String();
	String(const String& str);
	String(String&& str) LN_NOEXCEPT;
	String& operator=(const String& str);
	String& operator=(String&& str) LN_NOEXCEPT;

	String(const String& str, int begin);
	String(const String& str, int begin, int length);
	String(const Char* str);
	String(const Char* str, int length);
	String(const Char* begin, const Char* end);
	String(int count, Char ch);
	String(const StringRef& str);
	String(const Path& path);
	// TODO: Path&&

#ifdef LN_STRING_FROM_CHAR
	String(const char* str);
#endif

	/** 文字列が空であるかを確認します。 */
	bool isEmpty() const;

	/** C 言語としての文字列表現を取得します。 */
	const Char* c_str() const;

	/** 文字列の長さを取得します。 */
	int getLength() const;

	/** メモリを再確保せずに格納できる最大の要素数を取得します。 */
	int getCapacity() const;

	/** 文字列をクリアします。 */
	void clear();

	/** 文字列の長さを変更します。 */
	void resize(int newLength);
	void resize(int newLength, Char ch);

	/** サイズ変更の予定を指示します。 */
	void reserve(int size);

	void assign(const Char* str);
	void assign(const Char* str, int length);
	void assign(int count, Char ch);
	void assign(const StringRef& str);

	void append(const Char* str, int length);
	
	/**
		@brief		指定した文字列がこの文字列内に存在するかを判断します。
		@param[in]	str		: 検索文字列
		@param[in]	cs		: 大文字と小文字の区別設定
		@return		文字列が存在すれば true。str が空文字列である場合は必ず true となります。
	*/
	bool contains(const StringRef& str, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	bool contains(Char ch, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;					/**< @overload contains */

	/**
		@brief		文字列を検索し、見つかった最初の文字のインデックスを返します。
		@param[in]	str			: 検索文字列
		@param[in]	startIndex	: 検索を開始するインデックス (省略した場合は先頭から)
		@param[in]	cs			: 大文字と小文字の区別設定
		@return		見つからなかった場合は -1。str が空文字列である場合は 0。
	*/
	int indexOf(const StringRef& str, int startIndex = 0, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	int indexOf(Char ch, int startIndex = 0, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;

	/**
		@brief		文字列を検索し、最後に見つかったインデックスを返します。
		@param[in]	str			: 検索文字列
		@param[in]	startIndex	: 検索を開始するインデックス (-1 を指定すると、文字列の末尾から検索を開始する)
		@param[in]	count		: 検索する文字数 (-1 を指定すると、文字列の先頭まで検索する)
		@param[in]	cs			: 大文字と小文字の区別設定
		@return		見つかった文字列の開始インデックス。見つからなかった場合は -1。
		@details	startIndex の位置から文字列の先頭に向かう count 文字分の領域から str を検索します。
		@code
					String str = "abcdef";
					str.LastIndexOf("de");			// => 3
					str.LastIndexOf("bc", 2);		// => 1
					str.LastIndexOf("cd", 2);		// => -1	(検索範囲 "abc" の中に "cd" は存在しない)
					str.LastIndexOf("cd", 4, 3);	// => 2		(検索範囲 "cde" の中に "cd" は存在する)
					str.LastIndexOf("bc", 4, 3);	// => -1	(検索範囲 "cde" の中に "bc" は存在しない)
		@endcode
	*/
	int lastIndexOf(const StringRef& str, int startIndex = -1, int count = -1, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	int lastIndexOf(Char ch, int startIndex = -1, int count = -1, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;	/**< @overload lastIndexOf */

	/**
		@brief		この文字列の先頭が、指定した文字列と一致するかを判断します。
		@param[in]	str			: 検索文字列
		@details	str が空文字の場合は必ず true が返ります。
	*/
	bool startsWith(const StringRef& str, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	bool startsWith(Char ch, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;				/**< @overload startsWith */

	/**
		@brief		この文字列の末尾が、指定した文字列と一致するかを判断します。
		@param[in]	str			: 検索文字列
		@details	str が空文字の場合は必ず true が返ります。
		@code
					str = "file.txt";
					if (str.EndsWith(".txt")) {
						// 一致した
					}
		@endcode
	*/
	bool endsWith(const StringRef& str, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	bool endsWith(Char ch, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;					/**< @overload endsWith */

	/**
		@brief		文字列の部分文字列を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	start	: 開始文字インデックス
		@param[in]	count	: 文字数 (-1 の場合、末尾まで抽出する)
		@return		抽出された文字列
		@code
					String s("abcdef");
					s.Mid(2, 3)		=> "cde";
		@endcode
	*/
	StringRef substring(int start, int count = -1) const;

	/**
		@brief		文字列の左側(先頭)から指定した文字数を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	count	: 文字数
		@return		抽出された文字列
		@code
					String s(_T("abcdef"));
					s.Left(2)		=> _T("ab");
		@endcode
	*/
	String left(int count) const;

	/**
		@brief		文字列の右側(末尾)から指定した文字数を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	count	: 文字数
		@return		抽出された文字列
		@code
					String s(_T("abcdef"));
					s.Right(2)		=> _T("ef");
		@endcode
	*/
	String right(int count) const;

	/** 文字列の先頭と末尾の空白を全て削除します。 */
	String trim() const;

	/** 小文字を大文字に変換します。(ロケールの影響を受けません) */
	String toUpper() const;

	/** 大文字を小文字に変換します。(ロケールの影響を受けません) */
	String toLower() const;

	/** 先頭の文字を大文字、以降を小文字に変換します。(ロケールの影響を受けません) */
	String toTitleCase() const;

	/**
		@brief		この文字列から指定した文字をすべて取り除いた新しい文字列を返します。
		@param[in]	str		: 削除する文字列
		@param[in]	cs		: 大文字と小文字の区別設定
	*/
	String remove(const StringRef& str, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	String remove(Char ch, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	
	/**
		@brief		文字列の置換を行います。
		@param[in]	from	: 置換される文字列
		@param[in]	to		: from を置換する文字列
		@return		置換結果の文字列
		@details	from に一致するすべての文字列を to に置換します。
	*/
	String replace(const StringRef& from, const StringRef& to, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const;
	
	/**
		@brief		文字列をデリミタで分割します。
		@param[in]	delim	: デリミタ文字列
		@param[in]	option	: 分割方法
		@return		分割結果の文字列配列
		@detail		分割が発生しない場合は文字列全体を持つ要素数1の配列を返します。
					
					~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					auto tokens = String("a,b,c").split(",");		// => ["a", "b", "c"]
					auto tokens = String("a").split(",");			// => ["a"]
					auto tokens = String(",").split(",");			// => ["", ""]
					auto tokens = String("a::b").split("::");		// => ["a", "b"]
					~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	*/
	List<String> split(const StringRef& delim, StringSplitOptions option = StringSplitOptions::None) const;

	/** ローカルの char 型文字列表現に変換します。 */
	std::string toStdString() const;

	/** ローカルの wchar_t 型文字列表現に変換します。 */
	std::wstring toStdWString() const;


	/** 指定した文字列を連結します。 */
	static String concat(const StringRef& str1, const StringRef& str2);

	/**
		@brief		書式文字列と可変長引数リストから文字列を生成します。
		@param[in]	format		: 書式文字列 (printf の書式指定構文)
		@param[in]	...			: 引数リスト

		@details	指定できる書式はC言語標準の printf 等の書式です。
					~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					String str1 = String::SPrintf(_T("%d"), 100);			// => "100"
					~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					各書式指定子の詳細な仕様は環境に依存します。
					特にこれらの中には32ビットシステムと64ビットシステムとの間で移植性がはっきりしないものがありますので、
					そういったシステムで利用する場合はオーバーラン等に注意してください。

					こういった移植性や後述する引数リストにクラスの実体を指定できてしまう問題を回避するため、
					Format() を使用することを推奨します。

		@attention	引数リストに指定できるのは環境がサポートしているプリミティブ型だけです。
					String クラス等のクラスや構造体を指定した場合は未定義動作となります。
					~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
					String str1 = String::SPrintf(_T("%d"), 100);			// => "100"
					String str2 = String::SPrintf(_T("%ss"), str1);			// => NG (未定義動作)
					String str2 = String::SPrintf(_T("%ss"), str1.c_str());	// => "100s"
					~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	*/
	static String sprintf(const Char* format, ...);


	template<typename... TArgs>
	static String format(const StringRef& format, TArgs&&... args);
	template<typename... TArgs>
	static String format(const Locale& locale, const StringRef& format, TArgs&&... args);	/**< @overload format */

	
	/**
		@brief		この文字列と、指定した文字列を比較します。
		@param[in]	str1		: 比較文字列
		@param[in]	index1		: str1 内の部分文字列の開始位置
		@param[in]	str2		: 比較文字列
		@param[in]	index2		: str2 内の部分文字列の開始位置
		@param[in]	length		: 比較する文字数 (-1 の場合、GetLength() の値を使用します)
		@param[in]	cs			: 大文字と小文字の区別設定
		@return		str1 が str2 より小さい → 0 より小さい値
					str1 と str2 が等しい   → 0
					str1 が str2 より大きい → 0 より大きい値
	*/
	static int compare(const String& str1, const String& str2, CaseSensitivity cs = CaseSensitivity::CaseSensitive);
	static int compare(const StringRef& str1, int index1, const StringRef& str2, int index2, int length = -1, CaseSensitivity cs = CaseSensitivity::CaseSensitive);

	static String fromCString(const char* str, int length = -1, Encoding* encoding = nullptr);
	static String fromCString(const wchar_t* str, int length = -1);

	/** @name STL interface */
	/** @{ */
	//typedef Char* iterator;
	//typedef const Char* const_iterator;

	//iterator begin();
	//const_iterator begin() const;
	//iterator end();
	//const_iterator end() const;

	/** @{ */



	/// 現在の環境で定義されている改行文字列を取得する
	static const String& getNewLine();

	/// 空文字列を取得する
	static const String& getEmpty();

	int getByteCount() const { return getLength() * sizeof(Char); }

	uint32_t getHashCode() const;

	bool isSSO() const LN_NOEXCEPT { return !detail::getLSB<0>(static_cast<uint8_t>(m_data.sso.length)); }
	bool isNonSSO() const LN_NOEXCEPT { return detail::getLSB<0>(static_cast<uint8_t>(m_data.sso.length)); }

	const Char& operator[](int index) const;

	String& operator=(const StringRef& rhs);
	String& operator=(const Char* rhs);
	String& operator=(Char ch);
	String& operator=(const Path& rhs);

	String& operator+=(const String& rhs);
	String& operator+=(const StringRef& rhs);
	String& operator+=(const Char* rhs);
	String& operator+=(Char rhs);

private:
	static std::size_t const SSOCapacity = 15;//31;//sizeof(uint32_t) * 4 / sizeof(Char) - 1;

	// resource management
	void init() LN_NOEXCEPT;
	void release() LN_NOEXCEPT;
	void copy(const String& str);
	void move(String&& str) LN_NOEXCEPT;
	void reserveBuffer(int length);
	Char* lockBuffer(int requestSize);
	void unlockBuffer(int confirmedSize);
	Char* getBuffer();
	const Char* getBuffer() const;

	// sso operation
	void setSSOLength(int len);
	int getSSOLength() const;
	void setSSO();
	void setNonSSO();

	// utils
	template<typename TChar> void assignFromCStr(const TChar* str, int length = -1, bool* outUsedDefaultChar = nullptr, Encoding* encoding = nullptr);

	Encoding* getThisTypeEncoding() const;
	static ByteBuffer convertTo(const String& str, const Encoding* encoding, bool* outUsedDefaultChar = nullptr);

	union Data
	{
		detail::UStringCore*	core;

		struct SSO
		{
			Char		buffer[SSOCapacity];
			Char		length;	// ---xxxxy	: x=size y:flag(0=sso,1=non sso)
		} sso;
	} m_data;
};

/**
	@brief		
*/
class UStringHelper
{
public:
	static size_t strlen(const Char* str);
	static int compare(const Char* str1, const Char* str2);

	template<typename TChar>
	static void toStringInt8(int8_t v, TChar* outStr, int size);
};

/**
	@brief		ある文字列に対する部分文字列の参照を保持します。
*/
class StringRef
{
public:
	StringRef()
	{
		m_string = nullptr;
		m_str = nullptr;
		m_len = 0;
	}

	StringRef(const String& str)
		: StringRef()
	{
		m_string = &str;
		m_str = str.c_str();
		m_len = str.getLength();
	}

	StringRef(const String& str, int startIndex)
		: StringRef()
	{
		m_string = &str;
		m_str = str.c_str() + startIndex;
		m_len = str.getLength() - startIndex;
	}

	StringRef(const String& str, int startIndex, int len)
		: StringRef()
	{
		m_str = str.c_str() + startIndex;
		m_len = len;
	}

	StringRef(const Char* str)
		: StringRef()
	{
		m_str = str;
		m_len = UStringHelper::strlen(str);
	}

	StringRef(const Char* str, int len)
		: StringRef()
	{
		m_str = str;
		m_len = len;
	}

	StringRef(const Char* begin, const Char* end)
		: StringRef()
	{
		m_str = begin;
		m_len = end - begin;
	}

	StringRef(const Path& path);

	StringRef(const StringRef& str) = default;

	StringRef& operator=(const StringRef& str) = default;

	~StringRef()
	{
		clear();
	}

	int getLength() const
	{
		return m_len;
	}

	const Char* data() const
	{
		return m_str;
	}

	const Char* end() const
	{
		return data() + getLength();
	}

	String mid(int start, int count) const;

	bool IsNullOrEmpty() const { return (data() == nullptr || getLength() <= 0); }

	const Char& operator[](int index) const { return *(data() + index); }

	size_t getHashCode() const;

	// TODO: internal
	const Char* getBegin() const { return data(); }
	const Char* getEnd() const { return end(); }


private:
	const String*	m_string;
	const Char*	m_str;
	int				m_len;

	void clear()
	{
		m_string = nullptr;
		m_str = nullptr;
		m_len = 0;
	}

	//void copy(const StringRef& str)
	//{
	//	m_string = str.;
	//	m_str = nullptr;
	//	m_len = 0;
	//}
};




/**
	@brief		
*/
class UStringConvert
{
public:
	//static std::basic_string<TCHAR> toStdTString(const Char* str);
	// len: \0 を含まないよう素数
	// size: \0 を含む要素数
	// return: \0 を含まない変換後の要素数
	static int convertNativeString(const char* src, int srcLen, char* dst, int dstSize);
	static int convertNativeString(const char* src, int srcLen, wchar_t* dst, int dstSize);
	static int convertNativeString(const wchar_t* src, int srcLen, char* dst, int dstSize);
	static int convertNativeString(const wchar_t* src, int srcLen, wchar_t* dst, int dstSize);
	static int convertNativeString(const char16_t* src, int srcLen, char* dst, int dstSize);
	static int convertNativeString(const char16_t* src, int srcLen, wchar_t* dst, int dstSize);

	static void convertToStdString(const char* src, int srcLen, std::string* outString);
	static void convertToStdString(const char* src, int srcLen, std::wstring* outString);
	static void convertToStdString(const wchar_t* src, int srcLen, std::string* outString);
	static void convertToStdString(const wchar_t* src, int srcLen, std::wstring* outString);
	static void convertToStdString(const char16_t* src, int srcLen, std::string* outString);
	static void convertToStdString(const char16_t* src, int srcLen, std::wstring* outString);
/*
	static int getMaxNativeStringConverLength(const char* src, int srcLen, const char* dst);
	static int getMaxNativeStringConverLength(const char* src, int srcLen, const wchar_t* dst);
	static int getMaxNativeStringConverLength(const wchar_t* src, int srcLen, const char* dst);
	static int getMaxNativeStringConverLength(const wchar_t* src, int srcLen, const wchar_t* dst);
	static int getMaxNativeStringConverLength(const char16_t* src, int srcLen, const char* dst);
	static int getMaxNativeStringConverLength(const char16_t* src, int srcLen, const wchar_t* dst);*/
};





namespace fmt {

template<typename TChar>
class GenericFormatStringRef
{
public:
	GenericFormatStringRef();
	GenericFormatStringRef(const TChar* begin, const TChar* end);
	GenericFormatStringRef(const GenericFormatStringRef& str);

	bool isEmpty() const { return m_length == 0; }
	int getLength() const { return m_length; }
	const TChar* begin() const { return m_str; }
	const TChar* end() const { return m_str + m_length; }

	const TChar& operator[](int index) const { return *(m_str + index); }

private:
	const TChar*	m_str;
	int				m_length;
};

template<typename TChar>
class GenericFormatStringBuilder
{
public:
	GenericFormatStringBuilder();
	GenericFormatStringBuilder(TChar* buffer, size_t bufferSize);

	void clear();
	void appendChar(TChar ch);
	void appendChar(TChar ch, int count);
	void appendString(const TChar* str);
	void appendString(const TChar* str, int length);
	void appendString(const String& str);
	const TChar* c_str() const;
	int getLength() const;
	bool isFixedBufferOver() const { return m_fixedBufferOver; }

private:
	void appendIntenal(const TChar* str, int length);

	ByteBuffer	m_buffer;
	size_t		m_bufferUsed;
	byte_t*		m_fixedBuffer;
	size_t		m_fixedBufferSize;
	bool		m_fixedBufferOver;
};

template<typename TChar>
class GenericStringFormatter
{
public:
	GenericStringFormatter()
		: m_error()
		, m_errorPos(0)
	{}
	~GenericStringFormatter() {}

	void reportError(const char* message, int pos) { m_error = message; m_errorPos = pos; }
	bool hasError() const { return !m_error.empty(); }

public:	// TODO
	const std::locale* m_locale;
	GenericFormatStringBuilder<TChar>	m_sb;
	GenericFormatStringRef<TChar>	m_formatString;
	GenericFormatStringRef<TChar>	m_precision;

private:
	std::string	m_error;
	int			m_errorPos;
};

template<typename Formatter>
void formatArg(Formatter&, ...)
{
	assert(0);
	//static_assert(false, "[Lumino format string error] Cannot format argument. Please overload formatArg.");
}

} // namespace fmt



namespace detail {

class UStringCore
{
public:

	UStringCore()
		: m_refCount(1)
		, m_str(nullptr)
		, m_capacity(0)
		, m_length(0)
	{}

	~UStringCore()
	{
		delete[] m_str;
	}

	bool isShared() const LN_NOEXCEPT { return (m_refCount > 1); }
	void addRef() { ++m_refCount; }
	void release()
	{
		--m_refCount;
		if (m_refCount == 0)
		{
			delete this;
		}
	}

	Char* get() LN_NOEXCEPT { return m_str; }
	const Char* get() const LN_NOEXCEPT { return m_str; }
	int getLength() const LN_NOEXCEPT { return m_length; }
	int getCapacity() const { return m_capacity; }
	void reserve(int length)
	{
		assert(length >= 0);
		int size = length + 1;
		if (m_capacity < size)
		{
			Char* oldStr = m_str;
			int oldLen = m_length;

			m_str = LN_NEW Char[size];
			m_capacity = length;

			if (oldStr != nullptr)
			{
				memcpy(m_str, oldStr, std::min(length, oldLen) * sizeof(Char));
				delete oldStr;
			}
		}
	}
	void fixLength(int length)
	{
		m_str[length] = '\0';
		m_length = length;
	}
	void resize(int length)
	{
		reserve(length);
		fixLength(length);
	}
	void clear()
	{
		if (m_str != nullptr)
		{
			m_str[0] = '\0';
		}
		m_length = 0;
	}

private:
	std::atomic<int>	m_refCount;
	Char*				m_str;
	int					m_capacity;
	int					m_length;
};

} // namespace detail



//==============================================================================
// String
//==============================================================================

inline const Char* String::c_str() const
{
	return (isSSO()) ? m_data.sso.buffer : ((m_data.core) ? m_data.core->get() : _TT(""));
}

inline int String::getLength() const
{
	return (isSSO()) ? getSSOLength() : ((m_data.core) ? m_data.core->getLength() : 0);
}

inline int String::getCapacity() const
{
	//return (isSSO() || !m_data.core) ? SSOCapacity : m_data.core->getCapacity();
	return (isSSO()) ? SSOCapacity : ((m_data.core) ? m_data.core->getCapacity() : 0);
}

//inline String::iterator String::begin() { return (isSSO()) ? m_data.sso.buffer : ((m_data.core) ? m_data.core->get() : u""); }
//inline String::const_iterator String::begin() const { return c_str(); }
//inline String::iterator String::end() { return begin() + getLength(); }
//inline String::const_iterator String::end() const { return begin() + getLength(); }

inline const Char& String::operator[](int index) const { return getBuffer()[index]; }	// TODO: check range
inline String& String::operator=(const StringRef& rhs) { assign(rhs); return *this; }
inline String& String::operator=(const Char* rhs) { assign(rhs); return *this; }
inline String& String::operator=(Char ch) { assign(&ch, 1); return *this; }

inline String& String::operator+=(const String& rhs) { append(rhs.c_str(), rhs.getLength()); return *this; }
inline String& String::operator+=(const StringRef& rhs) { append(rhs.data(), rhs.getLength()); return *this; }
inline String& String::operator+=(const Char* rhs) { append(rhs, UStringHelper::strlen(rhs)); return *this; }
inline String& String::operator+=(Char rhs) { append(&rhs, 1); return *this; }

inline String operator+(const String& lhs, const String& rhs) { return String::concat(lhs, rhs); }
inline String operator+(const String& lhs, const Char* rhs) { return String::concat(lhs, StringRef(rhs)); }
inline String operator+(const Char* lhs, const String& rhs) { return String::concat(StringRef(lhs), rhs); }
inline String operator+(const String& lhs, Char rhs) { return String::concat(lhs, StringRef(&rhs, 1)); }
inline String operator+(Char lhs, const String& rhs) { return String::concat(StringRef(&lhs, 1), rhs); }

inline bool operator==(const Char* lhs, const String& rhs) { return UStringHelper::compare(lhs, rhs.c_str()) == 0; }
inline bool operator==(const String& lhs, const String& rhs) { return String::compare(lhs, 0, rhs, 0) == 0; }
inline bool operator==(const String& lhs, const Char* rhs) { return UStringHelper::compare(lhs.c_str(), rhs) == 0; }
inline bool operator==(const String& lhs, const StringRef& rhs) { return String::compare(lhs, 0, rhs, 0) == 0; }
inline bool operator!=(const Char* lhs, const String& rhs) { return !operator==(lhs, rhs); }
inline bool operator!=(const String& lhs, const String& rhs) { return !operator==(lhs, rhs); }
inline bool operator!=(const String& lhs, const Char* rhs) { return !operator==(lhs, rhs); }

inline bool operator<(const String& lhs, const String& rhs) { return String::compare(lhs, 0, rhs, 0, std::max(lhs.getLength(), rhs.getLength()), CaseSensitivity::CaseSensitive) < 0; }
inline bool operator<(const Char* lhs, const String& rhs) { return String::compare(lhs, 0, rhs, 0, -1, CaseSensitivity::CaseSensitive) < 0; }
inline bool operator<(const String& lhs, const Char* rhs) { return String::compare(lhs, 0, rhs, 0, -1, CaseSensitivity::CaseSensitive) < 0; }
inline bool operator>(const String& lhs, const String& rhs) { return String::compare(lhs, 0, rhs, 0, std::max(lhs.getLength(), rhs.getLength()), CaseSensitivity::CaseSensitive) > 0; }
inline bool operator>(const Char* lhs, const String& rhs) { return String::compare(lhs, 0, rhs, 0, -1, CaseSensitivity::CaseSensitive) > 0; }
inline bool operator>(const String& lhs, const Char* rhs) { return String::compare(lhs, 0, rhs, 0, -1, CaseSensitivity::CaseSensitive) > 0; }

inline bool operator<=(const String& lhs, const String& rhs) { return !operator>(lhs, rhs); }
inline bool operator<=(const Char* lhs, const String& rhs) { return !operator>(lhs, rhs); }
inline bool operator<=(const String& lhs, const Char* rhs) { return !operator>(lhs, rhs); }
inline bool operator>=(const String& lhs, const String& rhs) { return !operator<(lhs, rhs); }
inline bool operator>=(const Char* lhs, const String& rhs) { return !operator<(lhs, rhs); }
inline bool operator>=(const String& lhs, const Char* rhs) { return !operator<(lhs, rhs); }


//==============================================================================
// StringRef
//==============================================================================
inline String operator+(const StringRef& lhs, const StringRef& rhs) { return String::concat(lhs, rhs); }

inline bool operator==(const StringRef& lhs, const StringRef& rhs) { return String::compare(lhs, 0, rhs, 0, std::max(lhs.getLength(), rhs.getLength())) == 0; }
inline bool operator==(const Char* lhs, const StringRef& rhs) { return String::compare(StringRef(lhs), 0, rhs, 0, -1) == 0; }
inline bool operator==(const StringRef& lhs, const Char* rhs) { return String::compare(lhs, 0, StringRef(rhs), 0, -1) == 0; }
inline bool operator!=(const StringRef& lhs, const StringRef& rhs) { return !operator==(lhs, rhs); }
inline bool operator!=(const Char* lhs, const StringRef& rhs) { return !operator==(lhs, rhs); }
inline bool operator!=(const StringRef& lhs, const Char* rhs) { return !operator==(lhs, rhs); }


//==============================================================================
// String globals
//==============================================================================


/**
	@brief		この文字列を整数値に変換します。
	@param[in]	base		: 基数 (0、2、8、10、16 のいずれかであること)
	@return		変換結果の数値
	@details	次の書式に従い、文字列を数値に変換します。<br>
				[whitespace] [{+ | – }] [0 [{ x | X }]] [digits | letters]		<br>
				16 進数値のアルファベットは大文字と小文字を区別しません。		<br><br>
					
				基数に 0 を指定すると、文字列の先頭文字から基数を自動判別します。<br>
				"0x" または "0X" であれば 16 進数、"0" であれば 8 進数、それ以外であれば 10 進数です。
				基数に 8 または 16 が指定されている際、文字列の先頭は "0" または "0x" である必要はありません。

	@exception	InvalidFormatException	指定された基数に対して有効な桁を示す数字以外の文字が含まれていました。または、書式の前後に空白以外の文字が存在しました。
	@exception	OverflowException		数値に変換する際にオーバーフローが発生しました。
*/
extern int8_t	toInt8(const String& str, int base = 0);
extern int16_t	toInt16(const String& str, int base = 0);	///< @copydoc toInt8
extern int32_t	toInt32(const String& str, int base = 0);	///< @copydoc toInt8
extern int64_t	toInt64(const String& str, int base = 0);	///< @copydoc toInt8
extern uint8_t	toUInt8(const String& str, int base = 0);	///< @copydoc toInt8
extern uint16_t	toUInt16(const String& str, int base = 0);	///< @copydoc toInt8
extern uint32_t	toUInt32(const String& str, int base = 0);	///< @copydoc toInt8
extern uint64_t	toUInt64(const String& str, int base = 0);	///< @copydoc toInt8

/**
	@brief		この文字列を整数値に変換し、成否を返します。
	@param[in]	outValue	: 結果を格納する変数のポインタ (NULL を指定すると成否のみを返す)
	@param[in]	base		: 基数 (0、2、8、10、16 のいずれかであること)
	@return		正常に変換された場合は true。それ以外の場合は false。
	@details	例外が発生しない点を除けば ToInt8 等と同様です。
				大量のループの内部等、例外によるパフォーマンスへの影響が懸念される場合に使用してください。
	@see		ToInt8
*/
extern bool		tryToInt8(const String& str, int8_t* outValue, int base = 0);
extern bool		tryToInt16(const String& str, int16_t* outValue, int base = 0);		///< @copydoc tryToInt8
extern bool		tryToInt32(const String& str, int32_t* outValue, int base = 0);		///< @copydoc tryToInt8
extern bool		tryToInt64(const String& str, int64_t* outValue, int base = 0);		///< @copydoc tryToInt8
extern bool		tryToUInt8(const String& str, uint8_t* outValue, int base = 0);		///< @copydoc tryToInt8
extern bool		tryToUInt16(const String& str, uint16_t* outValue, int base = 0);		///< @copydoc tryToInt8
extern bool		tryToUInt32(const String& str, uint32_t* outValue, int base = 0);		///< @copydoc tryToInt8
extern bool		tryToUInt64(const String& str, uint64_t* outValue, int base = 0);		///< @copydoc tryToInt8


LN_NAMESPACE_END

// for unordered_map key
namespace std {
template <> struct hash<ln::String> { std::size_t operator () (const ln::String& key) const; };
}

#include "StringFormat.inl"
