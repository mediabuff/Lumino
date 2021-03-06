﻿
#include "../Internal.h"
#include <memory>
#if defined(LN_OS_WIN32) && _MSC_VER >= 1900
#include <cuchar>
#endif
#include <Lumino/Text/Encoding.h>
#include <Lumino/Base/ByteBuffer.h>
//#include <Lumino/IO/Common.h>	// TODO: for Path
//#include <Lumino/Base/Environment.h>	// TODO: Path
#include <Lumino/Base/String.h>
#include <Lumino/Base/StringHelper.h>
#include <Lumino/Base/RefObject.h>
#include <Lumino/Base/Formatter.h>
#include <Lumino/Base/Locale.h>
#include <Lumino/Base/Hash.h>
#include <Lumino/IO/PathName.h>
#include <Lumino/IO/PathTraits.h>	// TODO: Path
#include <Lumino/IO/FileSystem.h>	// TODO: Path
#include <Lumino/IO/DirectoryUtils.h>	// TODO: Path
#include <Lumino/Text/Encoding.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// String
//==============================================================================

String::String()
{
	init();
}

String::~String()
{
	release();
}

String::String(const String& str)
	: String()
{
	copy(str);
}

String::String(String&& str) LN_NOEXCEPT
	: String()
{
	move(std::forward<String>(str));
}

String& String::operator=(const String& str)
{
	copy(str);
	return *this;
}

String& String::operator=(String&& str) LN_NOEXCEPT
{
	move(std::forward<String>(str));
	return *this;
}

String::String(const String& str, int begin)
	: String()
{
	assign(str.c_str() + begin, str.getLength());
}

String::String(const String& str, int begin, int length)
	: String()
{
	assign(str.c_str() + begin, length);
}

String::String(const Char* str)
	: String()
{
	assign(str);
}

String::String(const Char* str, int length)
	: String()
{
	assign(str, length);
}

String::String(const Char* begin, const Char* end)
	: String()
{
	assign(begin, end - begin);
}

String::String(int count, Char ch)
	: String()
{
	assign(count, ch);
}

String::String(const StringRef& str)
	: String()
{
	assign(str);
}

String::String(const Path& path)
	: String(path.getString())
{
}

#ifdef LN_STRING_FROM_CHAR
String::String(const char* str)
	: String()
{
	assignFromCStr(str);
}
#endif

bool String::isEmpty() const
{
	if (isSSO())
	{
		return getSSOLength() == 0;
	}
	else
	{
		return !m_data.core || m_data.core->getLength() <= 0;
	}
}

void String::clear()
{
	lockBuffer(0);
	unlockBuffer(0);
}

void String::resize(int newLength)
{
	resize(newLength, Char());
}

void String::resize(int newLength, Char ch)
{
	int oldLen = getLength();
	Char* buf = lockBuffer(newLength);
	if (newLength > oldLen)
	{
		for (int i = oldLen; i < newLength; i++)
		{
			buf[i] = ch;
		}
	}
	unlockBuffer(newLength);
}

void String::reserve(int size)
{
	reserveBuffer(size);
}

bool String::contains(const StringRef& str, CaseSensitivity cs) const
{
	return indexOf(str, 0, cs) >= 0;
}

bool String::contains(Char ch, CaseSensitivity cs) const
{
	return indexOf(ch, 0, cs) >= 0;
}

int String::indexOf(const StringRef& str, int startIndex, CaseSensitivity cs) const
{
	return StringTraits::indexOf(c_str(), getLength(), str.data(), str.getLength(), startIndex, cs);
}

int String::indexOf(Char ch, int startIndex, CaseSensitivity cs) const
{
	return StringTraits::indexOf(c_str(), getLength(), &ch, 1, startIndex, cs);
}

int String::lastIndexOf(const StringRef& str, int startIndex, int count, CaseSensitivity cs) const
{
	return StringTraits::lastIndexOf(c_str(), getLength(), str.data(), str.getLength(), startIndex, count, cs);
}

int String::lastIndexOf(Char ch, int startIndex, int count, CaseSensitivity cs) const
{
	return StringTraits::lastIndexOf(c_str(), getLength(), &ch, 1, startIndex, count, cs);
}

bool String::startsWith(const StringRef& str, CaseSensitivity cs) const
{
	return StringTraits::startsWith(c_str(), getLength(), str.data(), str.getLength(), cs);
}

bool String::startsWith(Char ch, CaseSensitivity cs) const
{
	return StringTraits::endsWith(c_str(), getLength(), &ch, 1, cs);
}

bool String::endsWith(const StringRef& str, CaseSensitivity cs) const
{
	return StringTraits::endsWith(c_str(), getLength(), str.data(), str.getLength(), cs);
}

bool String::endsWith(Char ch, CaseSensitivity cs) const
{
	return StringTraits::endsWith(c_str(), getLength(), &ch, 1, cs);
}

StringRef String::substring(int start, int count) const
{
	int len = getLength();

	if (start < 0)
	{
		start = 0;
	}
	if (count < 0)
	{
		count = len - start;
	}
	if (start + count > len)
	{
		count = len - start;
	}
	if (start > len)
	{
		count = 0;
	}

	if (start == 0 && count == len)
	{
		return StringRef(c_str());
	}

	return StringRef(c_str() + start, count);
}

String String::left(int count) const
{
	const Char* begin;
	const Char* end;
	StringTraits::left(c_str(), count, &begin, &end);
	return String(begin, end);
}

String String::right(int count) const
{
	const Char* begin;
	const Char* end;
	StringTraits::right(c_str(), count, &begin, &end);
	return String(begin, end);
}

String String::trim() const
{
	const Char* begin;
	int length;
	StringTraits::trim(c_str(), getLength(), &begin, &length);
	return String(begin, length);
}

String String::toUpper() const
{
	int len = getLength();
	String result(c_str(), len);
	Char* buf = result.getBuffer();
	std::transform(buf, buf + len, buf, StringTraits::toUpper<Char>);
	return result;
}

String String::toLower() const
{
	int len = getLength();
	String result(c_str(), len);
	Char* buf = result.getBuffer();
	std::transform(buf, buf + len, buf, StringTraits::toLower<Char>);
	return result;
}

String String::toTitleCase() const
{
	int len = getLength();
	String result(c_str(), len);
	Char* buf = result.getBuffer();
	std::transform(buf, buf + len, buf, StringTraits::toLower<Char>);
	if (len > 0) buf[0] = StringTraits::toUpper<Char>(buf[0]);
	return result;
}

String String::remove(const StringRef& str, CaseSensitivity cs) const
{
	String result;
	const Char* pos = c_str();
	const Char* end = pos + getLength();
	const Char* fs = str.data();
	int fsLen = str.getLength();

	Char* buf = result.lockBuffer(end - pos);
	Char* bufBegin = buf;

	if (fsLen > 0)
	{
		for (; pos < end;)
		{
			int index = StringTraits::indexOf(pos, end - pos, fs, fsLen, 0, cs);
			if (index >= 0)
			{
				memcpy(buf, pos, index * sizeof(Char));
				buf += index;
				pos += index + fsLen;
			}
			else
			{
				break;
			}
		}
	}

	if (pos < end)
	{
		memcpy(buf, pos, (end - pos) * sizeof(Char));
		buf += (end - pos);
	}
	
	result.unlockBuffer(buf - bufBegin);
	return result;
}

String String::replace(const StringRef& from, const StringRef& to, CaseSensitivity cs) const
{
	String result;
	result.reserve(getLength());

	int pos = 0;
	const Char* src = c_str();
	int srcLen = getLength();
	const Char* fromStr = from.data();
	int fromLen = from.getLength();
	int start = 0;

	if (fromLen > 0)
	{
		do
		{
			pos = StringTraits::indexOf(src, srcLen, fromStr, fromLen, start, cs);
			if (pos >= 0)
			{
				result.append(src + start, pos - start);
				result.append(to.data(), to.getLength());
				start = pos + fromLen;
			}

		} while (pos >= 0);
	}

	result.append(src + start, srcLen - start);
	return result;
}

List<String> String::split(const StringRef& delim, StringSplitOptions option) const
{
	List<String> result;
	StringTraits::SplitHelper(
		c_str(), c_str() + getLength(), delim.data(), delim.getLength(), option, CaseSensitivity::CaseSensitive,
		[&result](const Char* begin, const Char* end) { result.add(String(begin, end - begin)); });
	return result;
}

std::string String::toStdString() const
{
	ByteBuffer buf = convertTo(*this, Encoding::getSystemMultiByteEncoding());
	return std::string((const char*)buf.getConstData());
}

std::wstring String::toStdWString() const
{
	ByteBuffer buf = convertTo(*this, Encoding::getWideCharEncoding());
	return std::wstring((const wchar_t*)buf.getConstData());
}

Encoding* String::getThisTypeEncoding() const
{
	if (sizeof(Char) == sizeof(char))
	{
		return Encoding::getSystemMultiByteEncoding();
	}
	else if (sizeof(Char) == sizeof(wchar_t))
	{
		return Encoding::getWideCharEncoding();
	}
	else if (sizeof(Char) == sizeof(char16_t))
	{
		return Encoding::getUTF16Encoding();
	}
	else
	{
		LN_NOTIMPLEMENTED();
	}
}

ByteBuffer String::convertTo(const String& str, const Encoding* encoding, bool* outUsedDefaultChar)
{
	Encoding* thisEncoding = str.getThisTypeEncoding();
	if (encoding == thisEncoding)	// TODO: ポインタ比較ではダメ。結果は正しいが。
	{
		return ByteBuffer(str.c_str(), (str.getLength() + 1) * sizeof(Char));
	}
	else
	{
		EncodingConversionOptions options;
		options.NullTerminated = true;

		EncodingConversionResult result;
		ByteBuffer buf = Encoding::convert(str.c_str(), str.getLength() * sizeof(Char), thisEncoding, encoding, options, &result);
		if (outUsedDefaultChar != nullptr)
		{
			*outUsedDefaultChar = result.UsedDefaultChar;
		}
		return buf;
	}
}

String String::remove(Char ch, CaseSensitivity cs) const
{
	return remove(StringRef(&ch, 1), cs);
}

String String::concat(const StringRef& str1, const StringRef& str2)
{
	String s;
	s.reserve(str1.getLength() + str2.getLength());
	s.append(str1.data(), str1.getLength());
	s.append(str2.data(), str2.getLength());
	return s;
}

String String::sprintf(const Char* format, ...)
{
	static const int MaxFormatLength = 256;

	va_list args1;
	va_start(args1, format);
	int len = StringTraits::tvscprintf_l(format, Locale::getC().getNativeLocale(), args1);	// 文字数を求める

	// 文字数が一定以内ならメモリ確保せずにスタックを使い、速度向上を図る
	if (len < MaxFormatLength)
	{
		Char buf[MaxFormatLength + 1];
		memset(buf, 0, sizeof(buf));
		StringTraits::tvsnprintf_l(buf, MaxFormatLength + 1, format, Locale::getDefault().getNativeLocale(), args1);
		va_end(args1);
		return String(buf);
	}
	else
	{
		ByteBuffer buf(sizeof(Char) * (len + 1));
		StringTraits::tvsnprintf_l((Char*)buf.getData(), len + 1, format, Locale::getDefault().getNativeLocale(), args1);
		va_end(args1);
		return String((Char*)buf.getData());
	}

}

int String::compare(const String& str1, const String& str2, CaseSensitivity cs)
{
	return StringTraits::compare(str1.c_str(), str1.getLength(), str2.c_str(), str2.getLength(), std::max(str1.getLength(), str2.getLength()), cs);
}

int String::compare(const StringRef& str1, int index1, const StringRef& str2, int index2, int length, CaseSensitivity cs)
{
	const Char* s1 = str1.data() + index1;
	const Char* s2 = str2.data() + index2;
	return StringTraits::compare(s1, str1.getLength() - index1, s2, str2.getLength() - index2, length, cs);
}

String String::fromCString(const char* str, int length, Encoding* encoding)
{
	String result;
	result.assignFromCStr(str, length, nullptr, encoding);
	return result;
}

String String::fromCString(const wchar_t* str, int length)
{
	String result;
	result.assignFromCStr(str, length, nullptr, nullptr);
	return result;
}

void String::init() LN_NOEXCEPT
{
	m_data.core = nullptr;
	m_data.sso.length = 0;
}

void String::release() LN_NOEXCEPT
{
	if (isNonSSO())
	{
		LN_SAFE_RELEASE(m_data.core);
	}
}

void String::copy(const String& str)
{
	if (this != &str)
	{
		if (isSSO())
		{
			if (str.isSSO())
			{
				// SSO -> SSO
				memcpy(&m_data, &str.m_data, sizeof(m_data));
			}
			else
			{
				// SSO -> NonSSO
				m_data.core = str.m_data.core;
				LN_SAFE_ADDREF(m_data.core);
				setNonSSO();
			}
		}
		else
		{
			if (str.isSSO())
			{
				// NonSSO -> SSO
				release();
				memcpy(&m_data, &str.m_data, sizeof(m_data));
			}
			else
			{
				// NonSSO -> NonSSO
				release();
				m_data.core = str.m_data.core;
				LN_SAFE_ADDREF(m_data.core);
			}
		}
	}
}

void String::move(String&& str) LN_NOEXCEPT
{
	if (isNonSSO())
	{
		LN_SAFE_RELEASE(m_data.core);
	}

	memcpy(&m_data, &str.m_data, sizeof(m_data));
	str.init();
}

Char* String::lockBuffer(int requestSize)
{
	reserveBuffer(requestSize);
	return getBuffer();
}

void String::unlockBuffer(int confirmedSize)
{
	if (isSSO())
	{
		setSSOLength(confirmedSize);
	}
	else
	{
		m_data.core->fixLength(confirmedSize);
	}
}

// 領域を確保し、sso かどうかのフラグをセットする。長さは変えない。
// 中身は以前のものが維持され、新しい領域は不定値となる。
void String::reserveBuffer(int length)
{
	if (isSSO())
	{
		if (length < SSOCapacity)
		{
			// SSO -> SSO
		}
		else
		{
			// SSO -> NonSSO
			std::unique_ptr<detail::UStringCore> core(LN_NEW detail::UStringCore());
			core->reserve(length);
			memcpy(core->get(), m_data.sso.buffer, std::min(getSSOLength(), length) * sizeof(Char));
			core->get()[length] = '\0';
			m_data.core = core.get();
			core.release();
			setNonSSO();
		}
	}
	else
	{
		if (m_data.core && !m_data.core->isShared() && length <= m_data.core->getCapacity())
		{
			// NonSSO で、共有されてもいないしサイズも間に合っているならいろいろ作り直す必要は無い
			m_data.core->get()[length] = '\0';
		}
		else
		{
			if (length < SSOCapacity)
			{
				// NonSSO -> SSO
				if (m_data.core)
				{
					detail::UStringCore* oldCore = m_data.core;
					setSSO();
					memcpy(m_data.sso.buffer, oldCore->get(), std::min(oldCore->getLength(), length) * sizeof(Char));
					oldCore->release();
				}
				else
				{
					setSSO();
				}
			}
			else
			{
				// NonSSO -> NonSSO
				if (m_data.core)
				{
					if (m_data.core->isShared())
					{
						detail::UStringCore* oldCore = m_data.core;
						m_data.core = LN_NEW detail::UStringCore();
						m_data.core->reserve(length);
						memcpy(m_data.core->get(), oldCore->get(), std::min(oldCore->getLength(), length) * sizeof(Char));
						oldCore->release();
					}
					else
					{
						m_data.core->reserve(length);
					}
				}
				else
				{
					m_data.core = LN_NEW detail::UStringCore();
					m_data.core->reserve(length);
				}
			}
		}
	}
}

Char* String::getBuffer()
{
	return (isSSO()) ? m_data.sso.buffer : m_data.core->get();
}

const Char* String::getBuffer() const
{
	return (isSSO()) ? m_data.sso.buffer : m_data.core->get();
}

void String::setSSOLength(int len)
{
	m_data.sso.length = (static_cast<size_t>(len) & 0x7F) << 1;
	m_data.sso.buffer[len] = '\0';
}

int String::getSSOLength() const
{
	return m_data.sso.length >> 1;
}

void String::setSSO()
{
	m_data.sso.length = (static_cast<size_t>(m_data.sso.length) & 0x07) << 1;
}

void String::setNonSSO()
{
	m_data.sso.length = 0x01;
}

void String::append(const Char* str, int length)
{
	int firstLen = getLength();
	Char* b = lockBuffer(firstLen + length) + firstLen;
	memcpy(b, str, length * sizeof(Char));
	unlockBuffer(firstLen + length);
}

void String::assign(const Char* str)
{
	assign(str, UStringHelper::strlen(str));
}

void String::assign(const Char* str, int length)
{
	if (str && *str)
	{
		Char* buf = lockBuffer(length);
		memcpy(buf, str, sizeof(Char) * length);
		unlockBuffer(length);
	}
	else
	{
		clear();
	}
}

void String::assign(int count, Char ch)
{
	if (count > 0)
	{
		Char* buf = lockBuffer(count);
		std::fill<Char*, Char>(buf, buf + count, ch);
		unlockBuffer(count);
	}
	else
	{
		clear();
	}
}

void String::assign(const StringRef& str)
{
	// TODO: String 参照のときの特殊化

	int len = str.getLength();
	if (len > 0)
	{
		Char* buf = lockBuffer(len);
		memcpy(buf, str.data(), sizeof(Char) * len);
		unlockBuffer(len);
	}
	else
	{
		clear();
	}
}

template<typename TChar>
void String::assignFromCStr(const TChar* str, int length, bool* outUsedDefaultChar, Encoding* encoding)
{
	int len = 0;
	bool ascii = true;

	if (str)
	{
		// ASCII だけの文字列か調べる。ついでに文字数も調べる。
		length = (length < 0) ? INT_MAX : length;
		const TChar* pos = str;
		for (; *pos && len < length; ++pos, ++len)
		{
			if (isascii(*pos) == 0)
			{
				ascii = false;
			}
		}
	}

	if (ascii)
	{
		Char* buf = lockBuffer(len);
		for (int i = 0; i < len; ++i)
		{
			buf[i] = str[i];
		}
		unlockBuffer(len);
	}
	else
	{
		if (!encoding)
		{
			encoding = Encoding::getEncodingTemplate<TChar>();
		}

		EncodingConversionOptions options;
		options.NullTerminated = false;

		EncodingConversionResult result;
		const ByteBuffer tmpBuffer = Encoding::convert(str, len * sizeof(TChar), encoding, getThisTypeEncoding(), options, &result);
		if (outUsedDefaultChar != nullptr) *outUsedDefaultChar = result.UsedDefaultChar;

		assign((const Char*)tmpBuffer.getData(), result.BytesUsed / sizeof(Char));
	}
}


uint32_t String::getHashCode() const
{
	if (isEmpty()) return 0;
	return ln::Hash::calcHash(c_str(), getLength());
}





const String& String::getNewLine()
{
#ifdef LN_OS_WIN32
	static String nl(_TT("\r\n"));
	return nl;
#elif defined(LN_OS_MAC)
	static String nl(_TT("\r"));
	return nl;
#else
	static String nl(_TT("\n"));
	return nl;
#endif
}

const String& String::getEmpty()
{
	static String str;
	return str;
}

String& String::operator=(const Path& rhs)
{
	assign(rhs.getString());
	return *this;
}

//==============================================================================
// StringRef
//==============================================================================

StringRef::StringRef(const Path& path)
	: StringRef(path.getString())
{
}

String StringRef::mid(int start, int count) const
{
	// TODO: Ref で返していいよね？
	//return StringTraits::mid(getBegin(), start, count);
	return String(getBegin(), count).substring(start, count);
}

size_t StringRef::getHashCode() const
{
	if (IsNullOrEmpty()) return 0;
	return Hash::calcHash(getBegin(), getLength());
}

//==============================================================================
// StringHelper
//==============================================================================

size_t UStringHelper::strlen(const Char* str)
{
	if (!str) return 0;
	size_t count = 0;
	for (; *str; ++str) ++count;
	return count;
}

int UStringHelper::compare(const Char* str1, const Char* str2)
{
	str1 = (str1) ? str1 : _TT("");
	str2 = (str2) ? str2 : _TT("");
	for (; *str1; ++str1, ++str2)
	{
		if (*str1 != *str2)
		{
			return (*str1 < *str2) ? -1 : 1;
		}
	}
	return (0);
}

template<typename TChar, typename TValue>
static void toStringIntX(TValue v, TChar* outStr, int size)
{
	char buf[64];
	detail::StdCharArrayBuffer<char> b(buf, 64);
	std::ostream os(&b);
	os << v;
	const char* str = b.GetCStr();
	int i = 0;
	for (; *str && i < (size - 1); ++str, ++i)
	{
		outStr[i] = *str;
	}
	outStr[i] = '\0';
}

template<typename TChar>
void UStringHelper::toStringInt8(int8_t v, TChar* outStr, int size)
{
	toStringIntX((int32_t)v, outStr, size);
}
template void UStringHelper::toStringInt8<char>(int8_t v, char* outStr, int size);
template void UStringHelper::toStringInt8<wchar_t>(int8_t v, wchar_t* outStr, int size);
template void UStringHelper::toStringInt8<char16_t>(int8_t v, char16_t* outStr, int size);



//==============================================================================
// UStringConvert
//==============================================================================

//std::basic_string<TCHAR> UStringConvert::toStdTString(const Char* str)
//{
//	String t = str;
//#ifdef LN_UNICODE
//	return t.toStdWString();
//#else
//	return t.toStdString();
//#endif
//}

int UStringConvert::convertNativeString(const char* src, int srcLen, char* dst, int dstSize)
{
	if (!dst || dstSize <= 0) return 0;
	if (src && srcLen >= 0)
	{
		int len = std::min(srcLen, dstSize -1);
		memcpy_s(dst, dstSize, src, len);
		dst[len] = '\0';
		return len;
	}
	else
	{
		dst[0] = '\0';
		return 0;
	}
}

int UStringConvert::convertNativeString(const char* src, int srcLen, wchar_t* dst, int dstSize)
{
	if (!dst || dstSize <= 0) return 0;
	if (src && srcLen >= 0)
	{
		if (srcLen >= dstSize) return -1;
		size_t size;
		errno_t err = mbstowcs_s(&size, dst, dstSize, src, srcLen);
		if (err != 0) return -1;
		return (int)size - 1;
	}
	else
	{
		dst[0] = '\0';
		return 0;
	}
}

int UStringConvert::convertNativeString(const wchar_t* src, int srcLen, char* dst, int dstSize)
{
	if (!dst || dstSize <= 0) return 0;
	if (src && srcLen >= 0)
	{
		if (srcLen >= dstSize) return -1;
		size_t size;
		errno_t err = wcstombs_s(&size, dst, dstSize, src, srcLen);
		if (err != 0) return -1;
		return (int)size - 1;
	}
	else
	{
		dst[0] = '\0';
		return 0;
	}
}

int UStringConvert::convertNativeString(const wchar_t* src, int srcLen, wchar_t* dst, int dstSize)
{
	if (!dst || dstSize <= 0) return 0;
	if (src && srcLen >= 0)
	{
		memcpy_s(dst, dstSize * sizeof(wchar_t), src, srcLen * sizeof(wchar_t));
		dst[dstSize - 1] = '\0';
		return dstSize - 1;
	}
	else
	{
		dst[0] = '\0';
		return 0;
	}
}

int UStringConvert::convertNativeString(const char16_t* src, int srcLen, char* dst, int dstSize)
{
#if defined(LN_OS_WIN32) && _MSC_VER >= 1900
	if (!dst || dstSize <= 0) return 0;
	if (src && srcLen >= 0)
	{
		mbstate_t state;
		char* p = dst;
		for (int n = 0; n < srcLen; ++n)
		{
			int rc = std::c16rtomb(p, src[n], &state);
			if (rc == -1) break;
			p += rc;
		}
		return p - dst;
	}
	else
	{
		dst[0] = '\0';
		return 0;
	}
#else
	// not found <uchar>
	LN_NOTIMPLEMENTED();
	return 0;
#endif
}

int UStringConvert::convertNativeString(const char16_t* src, int srcSize, wchar_t* dst, int dstSize)
{
#ifdef LN_WCHAR_16
	return convertNativeString((const wchar_t*)src, srcSize, dst, dstSize);
#else
	LN_NOTIMPLEMENTED();
	return -1;
#endif
}

//int UStringConvert::getMaxNativeStringConverLength(const char* src, int srcLen, const char* dst)
//{
//	return srcLen;
//}
//int UStringConvert::getMaxNativeStringConverLength(const char* src, int srcLen, const wchar_t* dst)
//{
//	int maxCodePoints = srcLen * 4;	// 最悪、UTF-8 と仮定
//	return maxCodePoints * 2;		// 最悪、全部サロゲートペアで構成されていると仮定
//}
//int UStringConvert::getMaxNativeStringConverLength(const wchar_t* src, int srcLen, const char* dst)
//{
//	
//}
//int UStringConvert::getMaxNativeStringConverLength(const wchar_t* src, int srcLen, const wchar_t* dst);
//int UStringConvert::getMaxNativeStringConverLength(const char16_t* src, int srcLen, const char* dst);
//int UStringConvert::getMaxNativeStringConverLength(const char16_t* src, int srcLen, const wchar_t* dst);


void UStringConvert::convertToStdString(const char* src, int srcLen, std::string* outString)
{
	outString->assign(src, srcLen);
}
void UStringConvert::convertToStdString(const char* src, int srcLen, std::wstring* outString)
{
	auto str = String::fromCString(src, srcLen);
	*outString = str.toStdWString();
}
void UStringConvert::convertToStdString(const wchar_t* src, int srcLen, std::string* outString)
{
	auto str = String::fromCString(src, srcLen);
	*outString = str.toStdString();
}
void UStringConvert::convertToStdString(const wchar_t* src, int srcLen, std::wstring* outString)
{
	outString->assign(src, srcLen);
}
void UStringConvert::convertToStdString(const char16_t* src, int srcLen, std::string* outString)
{
	LN_NOTIMPLEMENTED();
}
void UStringConvert::convertToStdString(const char16_t* src, int srcLen, std::wstring* outString)
{
	LN_NOTIMPLEMENTED();
}



//bool Path::operator < (const Path& right) const { return PathTraits::compare(m_path.c_str(), right.m_path.c_str()) < 0; }
//bool Path::operator < (const Char* right) const { return PathTraits::compare(m_path.c_str(), right) < 0; }

//namespace fmt {

////==============================================================================
//// GenericFormatStringBuilder
////==============================================================================
//
//template class GenericFormatStringBuilder<char16_t>;
//
//template<typename TChar>
//GenericFormatStringBuilder<TChar>::GenericFormatStringBuilder()
//	: m_buffer()
//	, m_bufferUsed(0)
//{
//}
//
//} // namespace fmt


//==============================================================================
// String globals
//==============================================================================

#define TO_INT_DEF(type, func) \
	const Char* begin; \
	const Char* end; \
	int len; \
	NumberConversionResult res; \
	StringTraits::trim(str.c_str(), str.getLength(), &begin, &len); \
	type num = StringTraits::func(begin, len, base, &end, &res); \
	if (res == NumberConversionResult::ArgsError)	{ LN_ENSURE(0); } \
	if (res == NumberConversionResult::FormatError)	{ LN_ENSURE(0); } \
	if (res == NumberConversionResult::Overflow)	{ LN_ENSURE(0); } \
	LN_ENSURE(end == begin + len); \
	return num;
int8_t toInt8(const String& str, int base) { TO_INT_DEF(int8_t, toInt8); }
int16_t toInt16(const String& str, int base) { TO_INT_DEF(int16_t, toInt16); }
int32_t toInt32(const String& str, int base) { TO_INT_DEF(int32_t, toInt32); }
int64_t toInt64(const String& str, int base) { TO_INT_DEF(int64_t, toInt64); }
uint8_t toUInt8(const String& str, int base) { TO_INT_DEF(uint8_t, toUInt8); }
uint16_t toUInt16(const String& str, int base) { TO_INT_DEF(uint16_t, toUInt16); }
uint32_t toUInt32(const String& str, int base) { TO_INT_DEF(uint32_t, toUInt32); }
uint64_t toUInt64(const String& str, int base) { TO_INT_DEF(uint64_t, toUInt64); }
#undef TO_INT_DEF

#define TRY_TO_INT_DEF(type, func) \
	const Char* begin; \
	const Char* end; \
	int len; \
	NumberConversionResult res; \
	StringTraits::trim(str.c_str(), str.getLength(), &begin, &len); \
	type num = StringTraits::func(begin, len, base, &end, &res); \
	if (end != begin + len) { return false; } \
	if (res != NumberConversionResult::Success) { return false; } \
	if (outValue != nullptr) { *outValue = num; } \
	return true;
bool tryToInt8(const String& str, int8_t* outValue, int base) { TRY_TO_INT_DEF(int8_t, toInt8); }
bool tryToInt16(const String& str, int16_t* outValue, int base) { TRY_TO_INT_DEF(int16_t, toInt16); }
bool tryToInt32(const String& str, int32_t* outValue, int base) { TRY_TO_INT_DEF(int32_t, toInt32); }
bool tryToInt64(const String& str, int64_t* outValue, int base) { TRY_TO_INT_DEF(int64_t, toInt64); }
bool tryToUInt8(const String& str, uint8_t* outValue, int base) { TRY_TO_INT_DEF(uint8_t, toUInt8); }
bool tryToUInt16(const String& str, uint16_t* outValue, int base) { TRY_TO_INT_DEF(uint16_t, toUInt16); }
bool tryToUInt32(const String& str, uint32_t* outValue, int base) { TRY_TO_INT_DEF(uint32_t, toUInt32); }
bool tryToUInt64(const String& str, uint64_t* outValue, int base) { TRY_TO_INT_DEF(uint64_t, toUInt64); }
#undef TRY_TO_INT_DEF

LN_NAMESPACE_END

//==============================================================================
//
//==============================================================================
namespace std {

// for unordered_map key
std::size_t hash<ln::String>::operator () (const ln::String& key) const
{
	return ln::Hash::calcHash(key.c_str(), key.getLength());
}

} // namespace std
