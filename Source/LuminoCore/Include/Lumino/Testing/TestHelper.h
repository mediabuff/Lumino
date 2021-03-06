﻿#pragma once
#include "../Base/String.h"
#include "../IO/PathName.h"

LN_NAMESPACE_BEGIN

class TestHelper
{
public:
	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static String getFilePath(const char* baseFilePath, const char* fileName);
	static String getFilePath(const char* baseFilePath, const Char* fileName) { return String(getFilePathU(baseFilePath, fileName).c_str()); }

	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static PathName getDirPath(const char* baseFilePath);

	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static std::string getFilePathA(const char* baseFilePath, const char* fileName);
	static std::wstring getFilePathW(const char* baseFilePath, const wchar_t* fileName);
	static Path getFilePathU(const char* baseFilePath, const Char* fileName);

	/// 2 つの配列をバイト単位で比較する
	static bool checkArrays(const void* ary1, const void* ary2, size_t count);

	// 2 つのファイル内容が一致するか確認する
	static bool equalFiles(const PathName& filePath1, const PathName& filePath2);
};

// __FILE__ のあるフォルダパス (PathName) を取得する
#define LN_TEST_GET_DIR_PATH() TestHelper::getDirPath(__FILE__)

// __FILE__ と同じフォルダのファイルパス (Char) を取得する
#define LN_LOCALFILE(fileName) TestHelper::getFilePath(__FILE__, fileName).c_str()

// __FILE__ と同じフォルダのファイルパス (char) を取得する
#define LN_LOCALFILEA(fileName) TestHelper::getFilePathA(__FILE__, fileName).c_str()

// __FILE__ と同じフォルダのファイルパス (wchar_t) を取得する
#define LN_LOCALFILEW(fileName) TestHelper::getFilePathW(__FILE__, fileName).c_str()

LN_NAMESPACE_END
