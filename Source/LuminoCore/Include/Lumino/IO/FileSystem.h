﻿/**
	@file	FileSystem.h
*/
#pragma once
#include <stdio.h>
#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "../Base/EnumExtension.h"
#include "DirectoryUtils.h"

LN_NAMESPACE_BEGIN
class Encoding;

/** ファイルとディレクトリの属性 */
LN_ENUM_FLAGS(FileAttribute)
{
	Normal		= 0x0000,	/**< 特に属性を持たない通常のファイル */
	Directory	= 0x0001,	/**< ディレクトリ */
	ReadOnly	= 0x0002,	/**< 読み取り専用 */
	Hidden		= 0x0004,	/**< 隠しファイル */
};
LN_ENUM_FLAGS_DECLARE(FileAttribute)

/**
	@brief	ファイルユーティリティ
*/
class FileSystem
{
public:
	/// fopen の template 実装
	//template<typename TChar>
	//static FILE* fopen(const TChar* filePath, const TChar* pMode);
	
	/**
		@brief		指定されたファイルが存在するか確認します。
		@details	この関数は指定されたファイルにアクセスできる場合に true を返します。
					例えば Windows では、実際にファイルが存在していてもそのファイルが
					別のパスワードロックされたユーザーフォルダ内に存在している場合は false を返します。
					(ユーザーA は ユーザーB の "マイドキュメント" フォルダのファイルにアクセスできない)
					また、パスが空文字や NULL の場合も false を返します。
	*/
	static bool existsFile(const StringRefA& filePath);
	static bool existsFile(const StringRefW& filePath);
	//template<typename TChar> static bool Exists2(const TChar* filePath);		/**< a */
	//template<typename TString> static bool Exists2(const TString& filePath);	/**< b */

	/**
		@brief		ファイルの属性を取得します。
		@param[in]	filePath		: ファイル名
		@return		ファイルの属性 (FileAttribute のビットの組み合わせ)
		@exception	FileNotFoundException	対象にアクセスできなかった。
	*/
	static FileAttribute getAttribute(const char* filePath);
	static FileAttribute getAttribute(const wchar_t* filePath);

	/**
		@brief		ファイルの属性を設定します。
		@param[in]	filePath		: ファイル名
		@param[in]	attr			: ファイルの属性 (FileAttribute のビットの組み合わせ)
		@details	この関数により変更できる属性は、読み取り属性のみです。(Unix では隠し属性はファイル名で表現されるためです)
					それ以外のフラグビットは無視されます。
	*/
	static void setAttribute(const char* filePath, FileAttribute attr);
	static void setAttribute(const wchar_t* filePath, FileAttribute attr);

	/**
		@brief		ファイルをコピーする
		@param[in]	sourceFileName	: コピー元ファイル名
		@param[in]	destFileName	: コピー先ファイル名
		@param[in]	overwrite		: コピー先を上書きする場合は true
		@details	読み取り専用ファイルに上書きすることはできません。
	*/
	static void copy(const char* sourceFileName, const char* destFileName, bool overwrite);
	static void copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite);

	/**
		@brief		ファイルを削除する
		@param[in]	filePath		: 削除するファイルのパス
		@details	削除するファイルが存在しない場合、例外はスローされません。
	*/
	static void deleteFile(const char* filePath);
	static void deleteFile(const wchar_t* filePath);

	/**
		@brief		ディレクトリを削除します。
		@param[in]	path			: 削除するディレクトリのパス
		@param[in]	recursive		: 
	*/
	static void deleteDirectory(const StringRefA& path, bool recursive) { deleteDirectoryInternal(path, recursive); }
	static void deleteDirectory(const StringRefW& path, bool recursive) { deleteDirectoryInternal(path, recursive); }

	template<typename TChar>
	static void deleteDirectoryInternal(const GenericStringRef<TChar>& path, bool recursive);


	/**
		@brief		
		@param[in]	overwrite		: コピー先のファイルとディレクトリを上書きする場合は true
	*/
	static void copyDirectory(const GenericStringRef<char>& srcPath, const GenericStringRef<char>& destPath, bool overwrite, bool recursive)
	{
		copyDirectoryInternal(srcPath, destPath, overwrite, recursive);
	}
	static void copyDirectory(const GenericStringRef<wchar_t>& srcPath, const GenericStringRef<wchar_t>& destPath, bool overwrite, bool recursive)
	{
		copyDirectoryInternal(srcPath, destPath, overwrite, recursive);
	}

	template<typename TChar>
	static void copyDirectoryInternal(const GenericStringRef<TChar>& srcPath, const GenericStringRef<TChar>& destPath, bool overwrite, bool recursive);











	
	/// ファイルサイズを取得する
	static uint64_t getFileSize(const TCHAR* filePath);

	/// ファイルサイズを取得する
	static uint64_t getFileSize(FILE* stream);

	/// ファイルの内容をすべて読み込む (バイナリ形式)
	static ByteBuffer readAllBytes(const StringRefA& filePath);
	static ByteBuffer readAllBytes(const StringRefW& filePath);

	/// 
	/// encoding 省略時は UTF8(BOM 無し)
	/**
		@brief		ファイルの内容をすべて読み込み、文字列として返します。
		@param[in]	filePath	: 読み込むファイルのパス
		@param[in]	encoding	: ファイルのエンコーディング
		@return		読み込んだ文字列
		@details	encoding が nullptr である場合、UTF8 テキストとして読み込みます。
					BOM の有無は自動判別します。
	*/
	static String readAllText(const StringRef& filePath, const Encoding* encoding = nullptr);

	/// 配列の内容をバイナリファイルとして書き出す
	static void writeAllBytes(const TCHAR* filePath, const void* buffer, size_t size);

	/// 文字列をテキストファイルとして書き出す
	/// encoding 省略時は UTF8 (BOM 無し)
	static void writeAllText(const TCHAR* filePath, const String& str, const Encoding* encoding = nullptr);

	
	/**
		@brief		指定したディレクトリが存在するかを確認します。
		@param[in]	path	: ディレクトリのパス
		@return		ディレクトリが存在すれば true。それ以外の場合は false。
	*/
	static bool existsDirectory(const char* path);
	static bool existsDirectory(const wchar_t* path);
	template<typename TString> static inline bool existsDirectory(const TString& path) { return existsDirectory(path.c_str()); }

	// TODO: これだけだと FileSystem::ForEachFilesInDirectory<TCHAR>() のように明示的な型指定が必要
	template<typename TChar, typename TCallback>
	static void forEachFilesInDirectory(const GenericStringRef<TChar>& path, TCallback callback);

#pragma push_macro("CreateDirectory")
#undef createDirectory
	/**
		@brief		ディレクトリを作成します。
		@param[in]	path	: 作成するディレクトリのパス
		@details	指定したパスへの全てのディレクトリを作成します。
					既に存在する場合は作成しません。
	*/
	static void createDirectory(const char* path);
	static void createDirectory(const wchar_t* path);

	static void LN_AFX_FUNCNAME(createDirectory)(const char* path);
	static void LN_AFX_FUNCNAME(createDirectory)(const wchar_t* path);
#pragma pop_macro("CreateDirectory")


	/// 現在の位置とデータ(ファイル)サイズ、オフセット、基準(SEEK_xxxx)を受け取って、新しいシーク位置を返す
	static int64_t calcSeekPoint(int64_t curPoint, int64_t maxSize, int64_t offset, int origin);
	
	/**
		@brief		現在の環境のファイルシステムが、パス文字列の大文字と小文字を区別するかを確認します。
	*/
	static CaseSensitivity getFileSystemCaseSensitivity();

private:
	static bool mkdir(const char* path);
	static bool mkdir(const wchar_t* path);
	static bool getAttributeInternal(const char* path, FileAttribute* outAttr);
	static bool getAttributeInternal(const wchar_t* path, FileAttribute* outAttr);
	template<typename TChar> static void createDirectoryInternal(const TChar* path);
};


//------------------------------------------------------------------------------
template<typename TChar, typename TCallback>
inline void FileSystem::forEachFilesInDirectory(const GenericStringRef<TChar>& path, TCallback callback)
{
	GenericFileFinder<TChar> finder(path);
	while (!finder.getCurrent().isEmpty())
	{
		callback(finder.getCurrent());
		finder.next();
	}
}

LN_NAMESPACE_END
