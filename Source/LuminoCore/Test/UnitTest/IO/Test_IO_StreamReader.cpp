﻿#include <TestConfig.h>
#include <Lumino/IO/StreamReader.h>
#include <Lumino/IO/MemoryStream.h>

class Test_IO_StreamReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_IO_StreamReader, readLine)
{
#if 0
	StreamReader reader(LN_LOCALFILE("TestData/Text_SJIS_CRLF.txt"), nullptr);
	String str, line;
	int count = 0;
	while (reader.ReadLine(&line))
	{
		if (!str.IsEmpty()) { str += _LT("\r\n"); }
		str += line;
		count++;
	}

	ByteBuffer buf = FileSystem::ReadAllBytes(LN_LOCALFILE("TestData/Text_UTF16LE_CRLF.txt"));
	ASSERT_EQ(0, ByteBuffer::Compare(buf, str.c_str(), str.GetByteCount()));
#endif

	// <Test> バッファリングバッファの境界で CRLF 改行文字が途切れていてもデコードできる。
	{
		char asciiBuf[StreamReader::DefaultBufferSize + 2];
		Char tcharBuf[StreamReader::DefaultBufferSize + 2];
		for (int i = 0; i < StreamReader::DefaultBufferSize; i++) {
			asciiBuf[i] = 'a';
			tcharBuf[i] = _LT('a');
		}
		asciiBuf[StreamReader::DefaultBufferSize - 1] = '\r';
		asciiBuf[StreamReader::DefaultBufferSize + 0] = '\n';
		asciiBuf[StreamReader::DefaultBufferSize + 1] = 'a';
		tcharBuf[StreamReader::DefaultBufferSize - 1] = _LT('\r');
		tcharBuf[StreamReader::DefaultBufferSize + 0] = _LT('\n');
		tcharBuf[StreamReader::DefaultBufferSize + 1] = _LT('a');

		MemoryStreamPtr mem = MemoryStream::create(asciiBuf, LN_ARRAY_SIZE_OF(asciiBuf));
		StreamReader reader(mem, Encoding::getEncoding(EncodingType::SJIS));

		String line1, line2, line3;
		ASSERT_TRUE(reader.readLine(&line1));
		ASSERT_TRUE(reader.readLine(&line2));
		ASSERT_FALSE(reader.readLine(&line3));

		ASSERT_EQ(StreamReader::DefaultBufferSize - 1, line1.getLength());
		ASSERT_EQ(0, String::compare(line1, String(tcharBuf, StreamReader::DefaultBufferSize - 1)));
		ASSERT_EQ(0, String::compare(line2, _LT("a")));
	}
}

