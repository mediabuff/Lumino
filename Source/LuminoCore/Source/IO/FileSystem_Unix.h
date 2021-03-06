﻿
#include <sys/types.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include "../Internal.h"
#include "../../Include/Lumino/Base/ByteBuffer.h"
#include "../../Include/Lumino/Text/Encoding.h"
#include "../../Include/Lumino/IO/FileStream.h"
#include "../../Include/Lumino/IO/FileSystem.h"
#include "../../Include/Lumino/IO/PathTraits.h"

LN_NAMESPACE_BEGIN

static bool is_stat_writable(struct stat *st, const char *path)
{
	// 制限なしに書き込み可であるか
	if (st->st_mode & S_IWOTH)
		return 1;
	// 現在のユーザーIDに許可されているか
	if ((st->st_uid == geteuid()) && (st->st_mode & S_IWUSR))
		return 1;
	// 現在のグループIDに許可されているか
	if ((st->st_gid == getegid()) && (st->st_mode & S_IWGRP))
		return 1;
	// もうここに来ることはほとんどないはずだが念のため
	return access(path, W_OK) == 0;
}

class PlatformFileSystem
{
public:
	using PathChar = char;
	using PathString = std::string;

	static bool existsFile(const char* filePath)
	{
		// ※fopen によるチェックはNG。ファイルが排他ロックで開かれていた時に失敗する。
		// ※access によるチェックもNG。ディレクトリも考慮してしまう。
		struct stat st;
		int ret = ::stat(filePath, &st);
		if (ret == 0)
		{
			if (S_ISDIR(st.st_mode)) {
				return false;		// ディレクトリだった。
			}
			return true;
		}
		return false;
	}

	static void setAttribute(const char* filePath, FileAttribute attr)
	{
		struct stat st;
		int ret = ::stat(filePath, &st);
		LN_THROW(ret != -1, IOException);

		// 変更できるのは読み取り属性だけ。
		// 隠し属性は Unix ではファイル名で表現する。
		if (attr.TestFlag(FileAttribute::ReadOnly)) {
			ret = ::chmod(filePath, st.st_mode & ~(S_IWUSR | S_IWOTH | S_IWGRP));
		}
		else {
			ret = ::chmod(filePath, st.st_mode | S_IWUSR);
		}
		LN_THROW(ret != -1, IOException);
	}

	static bool getAttribute(const char* path, FileAttribute* outAttr)
	{
		// Unix 系の場合、ファイルの先頭が . であれば隠しファイルである。
		// mono-master/mono/io-layer/io.c の、_wapi_stat_to_file_attributes が参考になる。
		struct stat st;
		int ret = ::stat(path, &st);
		if (ret == -1) {
			return false;
		}

		const char* fileName = PathTraits::getFileNameSub(path);
		FileAttribute attrs = FileAttribute::None;
		if (S_ISDIR(st.st_mode))
		{
			attrs |= FileAttribute::Directory;
			if (!is_stat_writable(&st, path)) {
				attrs |= FileAttribute::ReadOnly;
			}
			if (fileName[0] == '.') {
				attrs |= FileAttribute::Hidden;
			}
		}
		else
		{
			attrs |= FileAttribute::Normal;
			if (!is_stat_writable(&st, path)) {
				attrs |= FileAttribute::ReadOnly;
			}
			if (fileName[0] == '.') {
				attrs |= FileAttribute::Hidden;
			}
		}
		*outAttr = attrs;
		return true;
	}

	static void copyFile(const char* sourceFileName, const char* destFileName, bool overwrite)
	{
		// コピー先ファイルの存在確認
		if (!overwrite && existsFile(destFileName)) {
			LN_THROW(0, IOException);
		}

		// http://ppp-lab.sakura.ne.jp/ProgrammingPlacePlus/c/044.html
		FILE* fpSrc = fopen(sourceFileName, "rb");
		if (fpSrc == NULL) {
			LN_THROW(0, IOException);
		}

		FILE* fpDest = fopen(destFileName, "wb");
		if (fpDest == NULL) {
			LN_THROW(0, IOException);
		}

		// バイナリデータとして 1byte ずつコピー
		// (windows ではバッファリングが効くけど、それ以外はわからない。
		//  Linux とかで極端に遅くなるようならここでバッファリングも考える)
		while (1)
		{
			byte_t b;
			fread(&b, 1, 1, fpSrc);
			if (feof(fpSrc)) {
				break;
			}
			if (ferror(fpSrc)) {
				LN_THROW(0, IOException);
			}

			fwrite(&b, sizeof(b), 1, fpDest);
		}

		fclose(fpDest);
		fclose(fpSrc);
	}

	static void deleteFile(const char* filePath)
	{
		int ret = remove(filePath);
		if (ret == -1) LN_THROW(0, IOException, strerror(errno));
	}

	static bool createDirectory(const char* path)
	{
		int ret = ::mkdir(path, 0755);
		return ret != -1;
	}

	static void removeDirectory(const char* path)
	{
		int r = rmdir(path);
		LN_THROW(r == 0, IOException);
	}

	static bool matchPath(const char* filePath, const char* pattern)
	{
		return fnmatch(pattern, filePath, FNM_PATHNAME) == 0;
	}

	static int getCurrentDirectory(int bufferLength, char* outBuffer)
	{
		LN_NOTIMPLEMENTED();
		return 0;
	}
	
	static uint64_t getFileSize(const char* filePath)
	{
		struct stat stat_buf;
		int r = stat(filePath, &stat_buf);
		LN_THROW(r == 0, FileNotFoundException);
		return stat_buf.st_size;
	}

	static uint64_t getFileSize(FILE* stream)
	{
		struct stat stbuf;
		int handle = fileno(stream);
		if (handle == 0) {
			return 0;
		}
		if (fstat(handle, &stbuf) == -1) {
			return 0;
		}
		return stbuf.st_size;
	}

	static FILE* fopen(const char* path, const char* mode)
	{
		return ::fopen(path, mode);
	}
};

class PlatformFileFinderImpl
{
public:
	PlatformFileFinderImpl()
		: m_dir(NULL)
		, m_current()
	{}

	~PlatformFileFinderImpl()
	{
		if (m_dir != NULL)
		{
			closedir(m_dir);
		}
	}

	void initialize(const char* path, int len)
	{
		m_dir = opendir(path);
		LN_THROW(m_dir != NULL, IOException, path);
	}

	const std::string& getCurrentFileName() const
	{
		return m_current;
	}

	bool isWorking() const
	{
		return !m_current.empty();
	}

	bool next()
	{
		struct dirent* entry;
		do
		{
			entry = readdir(m_dir);
			if (entry)
			{
				m_current = entry->d_name;
			}
			else
			{
				m_current.clear();
				break;
			}

		} while (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0);

		return !m_current.empty();
	}

private:
	DIR*			m_dir;
	std::string		m_current;
};


LN_NAMESPACE_END
