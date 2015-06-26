﻿
#pragma once

#include "../../external/libpng/png.h"
#include "../../external/libpng/pnginfo.h"
#include <Lumino/Base/ByteBuffer.h>
#include <Lumino/Base/Rect.h>
#include <Lumino/IO/Stream.h>
#include "../../include/Lumino/Imaging/Bitmap.h"

namespace Lumino
{
namespace Imaging
{

/// png ファイル
class PngFile
{
public:

	PngFile()
		:/* m_bitmapData(NULL)
		, */mPngStruct(NULL)
		, mPngInfo(NULL)
	{
	}

	~PngFile()
	{
		if (mPngStruct) {
			png_destroy_read_struct(&mPngStruct, &mPngInfo, NULL);
		}
		//LN_SAFE_RELEASE(m_bitmapData);
	}


public:
	Size		m_size;
	//int						mWidth;
	//int						mHeight;
	ByteBuffer	m_bitmapData;
	PixelFormat	m_format;


	static const int PNG_BYTES_TO_CHECK = 4;	// png 識別用の、ファイルの先頭バイト数

	/// 読み込みコールバックに渡すデータ
	struct PngData
	{
		Stream* SourceStream;
		//unsigned char* Data;
		//unsigned long  Length;
		//unsigned long  Offset;
	};

	png_structp	mPngStruct;
	png_infop	mPngInfo;


public:

	/// 読み取り
	bool load(Stream* stream, bool swapHeight)
	{
		png_byte	sig[PNG_BYTES_TO_CHECK];
		//size_t		size;
		int			res;

		size_t dataSize = (size_t)stream->GetLength();

		// サイズチェック
		if (dataSize < PNG_BYTES_TO_CHECK) return false;

		// データが png かどうか調べる
		stream->Read(sig, 4);
		stream->Seek(0, SeekOrigin_Begin);
		res = png_check_sig(sig, PNG_BYTES_TO_CHECK);
		if (!res) return false;

		// png_struct構造体を確保・初期化する
		mPngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!mPngStruct) return false;

		// png_info構造体を確保・初期化する
		mPngInfo = png_create_info_struct(mPngStruct);
		if (!mPngInfo) return false;

		if (setjmp(png_jmpbuf(mPngStruct))) return false;

		// png データ、読み込みコールバック設定
		PngData png_data;
		//png_data.Data   = (unsigned char*)data;
		//png_data.Offset = 0;
		//png_data.Length = dataSize;
		png_data.SourceStream = stream;
		png_set_read_fn(mPngStruct, (void *)&png_data, _readDataForPng);

		// シグネチャの確認で読み飛ばしたバイト数を知らせる
		//png_set_sig_bytes( mPngStruct, PNG_BYTES_TO_CHECK );

		// PNGファイルのヘッダ情報を読み込む
		png_read_png(mPngStruct, mPngInfo, PNG_TRANSFORM_EXPAND, NULL);
		//png_read_info( mPngStruct, mPngInfo );

		// IHDRチャンク情報を取得する
		//png_get_IHDR(
		//          mPngStruct, mPngInfo, &mWidth, &mHeight,
		//	&mBitDepth, &mColorType, &mInterlaceType, NULL, NULL);

		m_size.Width = mPngInfo->width;
		m_size.Height = mPngInfo->height;



		// 必ず1色 8 ビットで
		if (mPngInfo->bit_depth != 8) return false;

		// パレットモードは非対応
		if (mPngInfo->color_type & PNG_COLOR_MASK_PALETTE) return false;

		//unsigned int row_bytes = png_get_rowbytes( mPngStruct, mPngInfo );
		//mImageData = (unsigned char*) malloc( row_bytes * mHeight );

		//png_bytepp row_pointers = png_get_rows( mPngStruct, mPngInfo );

		//for (int i = 0; i < mHeight; i++) {
		//    memcpy(mImageData+(row_bytes * (i)), row_pointers[i], row_bytes);
		//}

		//printf("%x\n", *((lnU32*)mImageData));
		//printf("%x\n", *(((lnU32*)mImageData) + 1));

		//-----------------------------------------------------
		// ビットマップ格納

		unsigned int	row_bytes = png_get_rowbytes(mPngStruct, mPngInfo);	// 横幅のバイト数 (例えば ABGR の時は 横幅 * 4)
		png_bytepp		row_pointers = png_get_rows(mPngStruct, mPngInfo);	// ビットマップデータ

		int sign = (swapHeight) ? -1 : 1;			// 反転するか？
		int unit = (swapHeight) ? m_size.Height - 1 : 0;	// イテレート開始行 (一番上か、一番下か)

		// ABGR
		// (R155, G128, B0, A78) のとき、U32(Little) で 4e0080ff となる。
		// byte[4] の並びは AA RR GG BB
		if (mPngInfo->color_type == PNG_COLOR_TYPE_RGB_ALPHA && mPngInfo->pixel_depth == 32)
		{
			m_format = PixelFormat_BYTE_R8G8B8A8;
			m_bitmapData = ByteBuffer(m_size.Width * m_size.Height * 4);
			byte_t* bitmap = m_bitmapData.GetData();

			// 1行ずつコピー
			for (int h = 0; h < m_size.Height; ++h) {
				memcpy(&bitmap[row_bytes * (unit + (sign * h))], row_pointers[h], row_bytes);
			}
		}
		// BGR
		// ABGR に拡張して読み込む
		else if (mPngInfo->color_type == PNG_COLOR_TYPE_RGB && mPngInfo->pixel_depth == 24)
		{
			m_format = PixelFormat_BYTE_R8G8B8A8;
			m_bitmapData = ByteBuffer(m_size.Width * m_size.Height * 4);
			byte_t* bitmap = m_bitmapData.GetData();

			byte_t* row;
			for (int y = 0; y < m_size.Height; ++y)
			{
				row = row_pointers[unit + (sign * y)];
				for (int x = 0; x < m_size.Width; ++x)
				{
					byte_t* src = &row[x * 3];
					byte_t* dest = &bitmap[(x + m_size.Width * y) * 4];
					dest[0] = src[0];	// R
					dest[1] = src[1];	// G
					dest[2] = src[2];	// B
					dest[3] = 0xFF;		// A
				}
			}
		}
		// Gray
		else if (mPngInfo->color_type == PNG_COLOR_TYPE_GRAY && mPngInfo->pixel_depth == 8)
		{
			m_format = PixelFormat_A8;
			m_bitmapData = ByteBuffer(m_size.Width * m_size.Height * 1);
			byte_t* bitmap = m_bitmapData.GetData();

			for (int h = 0; h < m_size.Height; ++h) {
				memcpy(&bitmap[row_bytes * (unit + (sign * h))], row_pointers[h], row_bytes);
			}
		}
		else {
			return false;
		}

		//PNG_COLOR_TYPE_GRAY// (ビット深度 1, 2, 4, 8, 16)
		//PNG_COLOR_TYPE_GRAY_ALPHA// (ビット深度 8, 16)
		//PNG_COLOR_TYPE_PALETTE// (ビット深度 1, 2, 4, 8)
		//PNG_COLOR_TYPE_RGB// (ビット深度 8, 16)
		//PNG_COLOR_TYPE_RGB_ALPHA// (ビット深度 8, 16)

		//PNG_COLOR_MASK_PALETTE
		//PNG_COLOR_MASK_COLOR
		//PNG_COLOR_MASK_ALPHA

		//if(png_get_valid(pPng, pInfo, PNG_INFO_tRNS))
		//	png_set_expand(pPng);
		//if(ColorType == PNG_COLOR_TYPE_PALETTE)
		//	png_set_expand(pPng);
		//if(ColorType == PNG_COLOR_TYPE_GRAY && bpp < 8)
		//	png_set_expand(pPng);
		//if(bpp > 8)
		//	png_set_strip_16(pPng);
		//if(ColorType == PNG_COLOR_TYPE_GRAY)
		//	png_set_gray_to_rgb(pPng);
		return true;
	}

	/// 保存 (bitmapData のフォーマットは PixelFormat_BYTE_R8G8B8A8 であること)
	void Save(const TCHAR* filePath, const ByteBuffer& bitmapData, const Size& size, bool upFlow)
	{
		FILE *fp;
		_tfopen_s(&fp, filePath, _T("wb"));

		png_structp pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		png_info* pngInfo = png_create_info_struct(pp);

		png_init_io(pp, fp);
		png_set_IHDR(pp, pngInfo, size.Width, size.Height,
			8,						// 各色 8bit
			PNG_COLOR_TYPE_RGBA,	// RGBA フォーマット
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);


		//png_bytepp rows = bitmapData->GetData();
		Array<png_bytep> rows;
		ByteBuffer tmpData = bitmapData;	// 書き込み可能ポインタでないと png の API に渡せないので一時メモリ化する。
		rows.Resize(size.Height);
		int rowBytes = png_get_rowbytes(pp, pngInfo);	// PixelFormat_BYTE_R8G8B8A8
		byte_t* data = tmpData.GetData();//bitmapData.GetData();
		for (int i = 0; i < size.Height; i++)
		{
			if (upFlow) {
				rows[size.Height - i - 1] = &data[rowBytes * i];
			}
			else {
				rows[i] = &data[rowBytes * i];
			}
		}


		// 書き込み
		png_write_info(pp, pngInfo);
		png_write_image(pp, &(rows[0]));
		png_write_end(pp, pngInfo);

		// 開放
		png_destroy_write_struct(&pp, &pngInfo);
		fclose(fp);
	}

private:

	/// ストリーム抽象化用
	static void _readDataForPng(png_structp png_ptr, png_bytep data, png_size_t length)
	{
#if 1
		PngData* png_data = (PngData*)png_get_io_ptr(png_ptr);
		int validSize = png_data->SourceStream->Read(data, length);
		if (validSize != length) {
			png_error(png_ptr, "_readPngData failed");
		}
#else
		PngData* buffer = (PngData*)png_get_io_ptr(png_ptr_);

		if (buffer->Offset + length_ <= buffer->Length)
		{
			memcpy(data_, buffer->Data + buffer->Offset, length_);
			buffer->Offset += length_;
		}
		else
		{
			printf("buffer->Offset:%d length_:%d buffer->Length:%d", buffer->Offset, length_, buffer->Length);
			png_error(png_ptr_, "_readPngData failed");
		}
#endif
	}
};

} // namespace Imaging
} // namespace Lumino
