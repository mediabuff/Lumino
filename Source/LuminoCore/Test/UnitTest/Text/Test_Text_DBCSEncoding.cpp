﻿#include <TestConfig.h>
#include <Lumino/IO/FileSystem.h>
#include <Lumino/Text/Encoding.h>

class Test_Text_DBCSEncoding : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Text_DBCSEncoding, SJIS)
{
	EncodingConversionOptions options;
	options.NullTerminated = false;

	Encoding* sjisEnc = Encoding::getEncoding(EncodingType::SJIS);
	ByteBuffer sjisBuf = FileSystem::readAllBytes(LN_LOCALFILE("TestData/ConvertTable_SJIS_test.txt"));
	ByteBuffer utf16Buf = FileSystem::readAllBytes(LN_LOCALFILE("TestData/ConvertTable_SJIS_test_UTF16.txt"));

	// MBCS → UTF16
	EncodingConversionResult result;
	ByteBuffer utf16Result = Encoding::convert(sjisBuf.getData(), sjisBuf.getSize(), sjisEnc, Encoding::getUTF16Encoding(), options, &result);

	int cmp = memcmp(utf16Buf.getData(), utf16Result.getData(), utf16Buf.getSize());
	ASSERT_EQ(0, cmp);											// 内容一致
	ASSERT_TRUE(utf16Buf.getSize() == utf16Result.getSize());	// バッファサイズ一致

	// UTF16 → MBCS
	ByteBuffer sjisResult = Encoding::convert(utf16Result.getData(), utf16Result.getSize(), Encoding::getUTF16Encoding(), sjisEnc, options, &result);

	cmp = memcmp(sjisBuf.getData(), sjisResult.getData(), sjisBuf.getSize());
	ASSERT_EQ(0, cmp);											// 内容一致
	ASSERT_TRUE(sjisBuf.getSize() == sjisResult.getSize());	// バッファサイズ一致
}
