﻿#include <TestConfig.h>


//==============================================================================
class Test_Base_StringUtils : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, StrLen)
{
	UTF8 str_u8[] = { 'A', 'B', 'C', 0x00 };
	UTF16 str_u16[] = { 'A', 'B', 'C', 0x00 };
	UTF32 str_u32[] = { 'A', 'B', 'C', 0x00 };

	ASSERT_EQ(3, StringTraits::tcslen(str_u8));
	ASSERT_EQ(3, StringTraits::tcslen(str_u16));
	ASSERT_EQ(3, StringTraits::tcslen(str_u32));
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toUpper)
{
	{
		UTF8 ch_u8 = 'a';
		UTF16 ch_u16 = 'a';
		UTF32 ch_u32 = 'a';

		ASSERT_EQ(0x41, StringTraits::toUpper(ch_u8));
		ASSERT_EQ(0x41, StringTraits::toUpper(ch_u16));
		ASSERT_EQ(0x41, StringTraits::toUpper(ch_u32));
	}
	{
		UTF8 ch_u8 = '+';
		UTF16 ch_u16 = '+';
		UTF32 ch_u32 = '+';

		ASSERT_EQ(0x2B, StringTraits::toUpper(ch_u8));
		ASSERT_EQ(0x2B, StringTraits::toUpper(ch_u16));
		ASSERT_EQ(0x2B, StringTraits::toUpper(ch_u32));
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, isSpace)
{
	{
		UTF8 str_u8[] = { '\t', '\n', '\v', '\f', '\r', ' ' };
		UTF16 str_u16[] = { '\t', '\n', '\v', '\f', '\r', ' ' };
		UTF32 str_u32[] = { '\t', '\n', '\v', '\f', '\r', ' ' };
		for (int i = 0; i < LN_ARRAY_SIZE_OF(str_u8); i++)
		{
			ASSERT_TRUE(StringTraits::isSpace(str_u8[i]));
			ASSERT_TRUE(StringTraits::isSpace(str_u16[i]));
			ASSERT_TRUE(StringTraits::isSpace(str_u32[i]));
		}
	}
	{
		UTF8 str_u8[] = { 'A' };
		UTF16 str_u16[] = { 'A' };
		UTF32 str_u32[] = { 'A' };
		for (int i = 0; i < LN_ARRAY_SIZE_OF(str_u8); i++)
		{
			ASSERT_FALSE(StringTraits::isSpace(str_u8[i]));
			ASSERT_FALSE(StringTraits::isSpace(str_u16[i]));
			ASSERT_FALSE(StringTraits::isSpace(str_u32[i]));
		}
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, compare)
{
	// <Test> 文字数指定無しの全体比較
	{
		ASSERT_EQ(true, StringTraits::compare("aa", -1, "aaa", -1, 2) == 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "aa", -1, 2) == 0);
		ASSERT_EQ(true, StringTraits::compare("aa", -1, "aaa", -1, 3) < 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "aa", -1, 3) > 0);
	}
	// <Test> 文字数指定有りの全体比較
	{
		ASSERT_EQ(true, StringTraits::compare("aa", 2, "aaa", 3, 2) == 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", 3, "aa", 2, 2) == 0);
		ASSERT_EQ(true, StringTraits::compare("aa", 2, "aaa", 3, 3) < 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", 3, "aa", 2, 3) > 0);
	}
	// <Test> 文字数指定無し・比較数指定無し
	{
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "aaa", -1, -1) == 0);
		ASSERT_EQ(true, StringTraits::compare("aa", -1, "aaa", -1, -1) < 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "aa", -1, -1) > 0);
	}
	// <Test> 文字数指定有りの比較数指定
	{
		ASSERT_EQ(true, StringTraits::compare("aa", 2, "aaa", 3, -1) < 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", 3, "aa", 2, -1) > 0);
	}
	// <Test> 部分比較
	{
		ASSERT_EQ(true, StringTraits::compare("aaa", 3, "aaa", 3, 4) == 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", 2, "aaa", 3, 4) < 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", 3, "aaa", 2, 4) > 0);
	}
	// <Test> デフォルト引数では大文字小文字を区別する
	{
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "aaa", -1, -1) == 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "AAA", -1, -1) != 0);
	}
	// <Test> 大文字小文字を区別する
	{
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "aaa", -1, -1, CaseSensitivity::CaseSensitive) == 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "AAA", -1, -1, CaseSensitivity::CaseSensitive) != 0);
	}
	// <Test> 大文字小文字を区別しない
	{
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "aaa", -1, -1, CaseSensitivity::CaseInsensitive) == 0);
		ASSERT_EQ(true, StringTraits::compare("aaa", -1, "AAA", -1, -1, CaseSensitivity::CaseInsensitive) == 0);
	}

	// <Test> 空文字列の比較
	{
		ASSERT_EQ(true, StringTraits::compare("", -1, "", -1, 0) == 0);
		ASSERT_EQ(true, StringTraits::compare("", 0, "", 0, 0) == 0);
		ASSERT_EQ(true, StringTraits::compare("", -1, "a", -1, 0) == 0);
		ASSERT_EQ(true, StringTraits::compare("a", -1, "", -1, 0) == 0);
		ASSERT_EQ(true, StringTraits::compare("", 0, "a", 1, 0) == 0);
		ASSERT_EQ(true, StringTraits::compare("a", 1, "", 0, 0) == 0);

		ASSERT_EQ(true, StringTraits::compare("", -1, "a", -1, 1) < 0);
		ASSERT_EQ(true, StringTraits::compare("a", -1, "", -1, 1) > 0);
		ASSERT_EQ(true, StringTraits::compare("", 0, "a", 1, 1) < 0);
		ASSERT_EQ(true, StringTraits::compare("a", 1, "", 0, 1) > 0);
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, endsWith)
{
	// 普通に比較
	{
		const Char* str1 = _LT("abc def");
		const Char* str2 = _LT("def");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}
	// 完全一致
	{
		const Char* str1 = _LT("def");
		const Char* str2 = _LT("def");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}
	// 不一致 (後ろ方向)
	{
		const Char* str1 = _LT("def");
		const Char* str2 = _LT("de");
		ASSERT_EQ(false, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}
	// 一致 (前方向方向)
	{
		const Char* str1 = _LT("def");
		const Char* str2 = _LT("ef");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}
	// 不一致 (str2が長い)
	{
		const Char* str1 = _LT("def");
		const Char* str2 = _LT("defghi");
		ASSERT_EQ(false, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}
	// 空文字どうし
	{
		const Char* str1 = _LT("");
		const Char* str2 = _LT("");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}
	// 空文字
	{
		const Char* str1 = _LT("a");
		const Char* str2 = _LT("");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}
	// 空文字
	{
		const Char* str1 = _LT("");
		const Char* str2 = _LT("a");
		ASSERT_EQ(false, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseSensitive));
	}

	// 普通に比較 (IgnoreCase)
	{
		const Char* str1 = _LT("abc def");
		const Char* str2 = _LT("DeF");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
	// 完全一致 (IgnoreCase)
	{
		const Char* str1 = _LT("DEF");
		const Char* str2 = _LT("def");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
	// 不一致 (後ろ方向) (IgnoreCase)
	{
		const Char* str1 = _LT("def");
		const Char* str2 = _LT("DE");
		ASSERT_EQ(false, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
	// 一致 (前方向方向) (IgnoreCase)
	{
		const Char* str1 = _LT("DEF");
		const Char* str2 = _LT("ef");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
	// 不一致 (str2が長い) (IgnoreCase)
	{
		const Char* str1 = _LT("def");
		const Char* str2 = _LT("defGHI");
		ASSERT_EQ(false, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
	// 空文字どうし (IgnoreCase)
	{
		const Char* str1 = _LT("");
		const Char* str2 = _LT("");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
	// 空文字 (IgnoreCase)
	{
		const Char* str1 = _LT("A");
		const Char* str2 = _LT("");
		ASSERT_EQ(true, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
	// 空文字 (IgnoreCase)
	{
		const Char* str1 = _LT("");
		const Char* str2 = _LT("A");
		ASSERT_EQ(false, StringTraits::endsWith(str1, -1, str2, -1, CaseSensitivity::CaseInsensitive));
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, countString)
{
	ASSERT_EQ(1, StringTraits::countString(_LT("abc"), -1, _LT("ab"), -1));
	ASSERT_EQ(2, StringTraits::countString(_LT("CaseSensitivity"), -1, _LT("it"), -1));
	ASSERT_EQ(0, StringTraits::countString(_LT("abc"), -1, _LT("abcd"), -1));
	ASSERT_EQ(2, StringTraits::countString(_LT("*****"), -1, _LT("**"), -1));
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toInt8)
{
	int8_t n;
	const Char* end;
	NumberConversionResult r;

	// min Overflow
	n = StringTraits::toInt8(_LT("-129"), -1, 0, &end, &r);
	ASSERT_EQ(-128, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);

	// min Success
	n = StringTraits::toInt8(_LT("-128"), -1, 0, &end, &r);
	ASSERT_EQ(-128, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toInt8(_LT("127"), -1, 0, &end, &r);
	ASSERT_EQ(127, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toInt8(_LT("128"), -1, 0, &end, &r);
	ASSERT_EQ(127, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toUInt8)
{
	uint8_t n;
	const Char* end;
	NumberConversionResult r;

	// Neg Success
	n = StringTraits::toUInt8(_LT("-1"), -1, 0, &end, &r);
	ASSERT_EQ(0xFF, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// min Success
	n = StringTraits::toUInt8(_LT("0"), -1, 0, &end, &r);
	ASSERT_EQ(0, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toUInt8(_LT("255"), -1, 0, &end, &r);
	ASSERT_EQ(255, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toUInt8(_LT("256"), -1, 0, &end, &r);
	ASSERT_EQ(255, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toInt16)
{
	int16_t n;
	const Char* end;
	NumberConversionResult r;

	// min Overflow
	n = StringTraits::toInt16(_LT("-32769"), -1, 0, &end, &r);
	ASSERT_EQ(-32768, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);

	// min Success
	n = StringTraits::toInt16(_LT("-32768"), -1, 0, &end, &r);
	ASSERT_EQ(-32768, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toInt16(_LT("32767"), -1, 0, &end, &r);
	ASSERT_EQ(32767, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toInt16(_LT("32768"), -1, 0, &end, &r);
	ASSERT_EQ(32767, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toUInt16)
{
	uint16_t n;
	const Char* end;
	NumberConversionResult r;

	// Neg Success
	n = StringTraits::toUInt16(_LT("-1"), -1, 0, &end, &r);
	ASSERT_EQ(0xFFFF, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// min Success
	n = StringTraits::toUInt16(_LT("0"), -1, 0, &end, &r);
	ASSERT_EQ(0, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toUInt16(_LT("65535"), -1, 0, &end, &r);
	ASSERT_EQ(65535, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toUInt16(_LT("65536"), -1, 0, &end, &r);
	ASSERT_EQ(65535, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toInt32)
{
	int32_t n;
	const Char* end;
	NumberConversionResult r;

	// min Overflow
	n = StringTraits::toInt32(_LT("-2147483649"), -1, 0, &end, &r);
	ASSERT_EQ(INT32_MIN, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);

	// min Success
	n = StringTraits::toInt32(_LT("-2147483648"), -1, 0, &end, &r);
	ASSERT_EQ(INT32_MIN, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toInt32(_LT("2147483647"), -1, 0, &end, &r);
	ASSERT_EQ(INT32_MAX, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toInt32(_LT("2147483648"), -1, 0, &end, &r);
	ASSERT_EQ(INT32_MAX, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toUInt32)
{
	uint32_t n;
	const Char* end;
	NumberConversionResult r;

	// Neg Success
	n = StringTraits::toUInt32(_LT("-1"), -1, 0, &end, &r);
	ASSERT_EQ(UINT32_MAX, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// min Success
	n = StringTraits::toUInt32(_LT("0"), -1, 0, &end, &r);
	ASSERT_EQ(0, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toUInt32(_LT("4294967295"), -1, 0, &end, &r);
	ASSERT_EQ(UINT32_MAX, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toUInt32(_LT("4294967296"), -1, 0, &end, &r);
	ASSERT_EQ(UINT32_MAX, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toInt64)
{
	int64_t n;
	const Char* end;
	NumberConversionResult r;

	// min Overflow
	n = StringTraits::toInt64(_LT("-9223372036854775809"), -1, 0, &end, &r);
	ASSERT_EQ(INT64_MIN, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);

	// min Success
	n = StringTraits::toInt64(_LT("-9223372036854775808"), -1, 0, &end, &r);
	ASSERT_EQ(INT64_MIN, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toInt64(_LT("9223372036854775807"), -1, 0, &end, &r);
	ASSERT_EQ(INT64_MAX, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toInt64(_LT("9223372036854775808"), -1, 0, &end, &r);
	ASSERT_EQ(INT64_MAX, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toUInt64)
{
	uint64_t n;
	const Char* end;
	NumberConversionResult r;

	// Neg Success
	n = StringTraits::toUInt64(_LT("-1"), -1, 0, &end, &r);
	ASSERT_EQ(UINT64_MAX, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// min Success
	n = StringTraits::toUInt64(_LT("0"), -1, 0, &end, &r);
	ASSERT_EQ(0, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Success
	n = StringTraits::toUInt64(_LT("0xffffffffffffffff"), -1, 0, &end, &r);
	ASSERT_EQ(UINT64_MAX, n);
	ASSERT_EQ(NumberConversionResult::Success, r);

	// max Overflow
	n = StringTraits::toUInt64(_LT("0xfffffffffffffffff"), -1, 0, &end, &r);
	ASSERT_EQ(UINT64_MAX, n);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, ToUInt)
{
	const Char* str;
	const Char* end;
	NumberConversionResult r;

	// 基数指定で 2 進数
	ASSERT_EQ(123, StringTraits::toInt32(_LT("1111011"), -1, 2));

	// 基数指定で 10 進数
	ASSERT_EQ(123, StringTraits::toInt32(_LT("123"), -1, 10));

	// 8進数として自動判別
	ASSERT_EQ(8, StringTraits::toInt32(_LT("010")));

	// 基数指定で 8 進数
	ASSERT_EQ(8, StringTraits::toInt32(_LT("010"), -1, 8, &end, &r));

	// 16進数として自動判別
	ASSERT_EQ(16, StringTraits::toInt32(_LT("0x10")));

	// 基数指定で 16進数
	ASSERT_EQ(16, StringTraits::toInt32(_LT("0x10"), -1, 16, &end, &r));

	// 16進数大文字
	ASSERT_EQ(0xABCDEF, StringTraits::toInt32(_LT("0xABCDEF")));

	// 16進数小文字
	ASSERT_EQ(0xABCDEF, StringTraits::toInt32(_LT("0xabcdef")));

	// 符号
	ASSERT_EQ(-10, StringTraits::toInt32(_LT("-10")));
	ASSERT_EQ(10, StringTraits::toInt32(_LT("+10")));

	// 前方に変な文字
	str = _LT("R1");
	StringTraits::toInt32(str, -1, 0, &end, &r);
	ASSERT_EQ(NumberConversionResult::Success, r);
	ASSERT_TRUE(str == end);

	// 後方に変な文字
	str = _LT("1R");
	StringTraits::toInt32(str, -1, 0, &end, &r);
	ASSERT_EQ(NumberConversionResult::Success, r);
	ASSERT_TRUE(str + 1 == end);

	// 文字数指定
	ASSERT_EQ(12, StringTraits::toInt32(_LT("123"), 2));

	/*
	https://msdn.microsoft.com/ja-jp/library/dwhawy9k(v=vs.110).aspx#FFormatString
	//double dd = 123.12345678901234;     // -> 123.123456789012
	//double dd = 5123.12345678901234;    // -> 5123.12345678901
	// double dd = 12.005;    // -> 12.005
	//double dd = 1.005;    // -> 1.005
	//double dd = 0.005;    // -> 0.005
	//double dd = .0000023;   // ->2.3E-06
	//double dd = 0.000023;   // ->2.3E-05
	// double dd = 0.00023;   // ->0.00023
	//double dd = 0.000235;   // ->0.000235
	double dd = 5.0000023;   // ->5.0000023
	var s = dd.ToString();
	*/
	//printf("%f", 5.000000001);
	//_itoa(10, );

	//const char* str2 = "1.5";
	//char* end2;
	//double d = strtod(str2, &end2);

	//printf("%f", 5.000000001);
	//DBL_MANT_DIG;
	//DBL_MAX_EXP;
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, toDouble)
{
	const Char* str;
	const Char* end;
	NumberConversionResult r;

	// 普通に
	str = _LT("1.52");
	ASSERT_DOUBLE_EQ(1.52, StringTraits::toDouble(str, -1, &end, &r));
	ASSERT_EQ(NumberConversionResult::Success, r);
	ASSERT_TRUE(str + 4 == end);

	// 文字数指定
	str = _LT("1.52");
	ASSERT_DOUBLE_EQ(1.5, StringTraits::toDouble(str, 3, &end, &r));
	ASSERT_EQ(NumberConversionResult::Success, r);
	ASSERT_TRUE(str + 3 == end);

	// オーバーフロー
	str = _LT("1.52E+999");
	StringTraits::toDouble(str, -1, &end, &r);
	ASSERT_EQ(NumberConversionResult::Overflow, r);

	// オーバーフロー
	str = _LT("1.52E-999");
	StringTraits::toDouble(str, -1, &end, &r);
	ASSERT_EQ(NumberConversionResult::Overflow, r);
}

