﻿#include <TestConfig.h>
#include <Lumino/Text/Encoding.h>

class Test_Text_UTF32Encoding : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Text_UTF32Encoding, AsciiCode)
{
	// <Test> ASCII 文字コードの通常文字部分をすべて変換してみる。
	{
		char ascii[95];
		for (int i = 0; i < 95; ++i) { ascii[i] = 0x20 + i; }
		UTF32 utf32[95];

		EncodingConversionResult result;
		Encoding::convert(ascii, 95, Encoding::getEncoding(EncodingType::ASCII), utf32, sizeof(utf32), Encoding::getUTF32Encoding(), &result);
	
		for (int i = 0; i < 95; ++i) {
			ASSERT_EQ(0x20 + i, utf32[i]);
		}
	}
}
