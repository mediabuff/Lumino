
#pragma once

#if defined(LUMINO_BUILD_DLL) && defined(LN_MSVC)
	#define LN_API	__declspec(dllexport)
#else
	#define LN_API
#endif

#ifdef LN_UNICODE
	typedef wchar_t		LNChar;
	#define __T(x)      L ## x
#else
	typedef char		LNChar;
	#define __T(x)      x
#endif

#define LN_OK	LN_RESULTCODE_OK

typedef intptr_t LNUserData;

/** 結果・エラーコード */
typedef enum tagLNResultCode
{
    LN_RESULTCODE_OK = 0,
    LN_RESULTCODE_ERRORUNKNOWN = 1,
    LN_RESULTCODE_ERRORVERIFY = 2,
    LN_RESULTCODE_ERRORARGUMENT = 3,
    LN_RESULTCODE_ERRORINVALID_OPERATION = 4,
    LN_RESULTCODE_ERRORNOT_IMPLEMENTED = 5,
    LN_RESULTCODE_ERROROUT_OF_MEMORY = 6,
    LN_RESULTCODE_ERROROUT_OF_RANGE = 7,
    LN_RESULTCODE_ERRORKEY_NOT_FOUND = 8,
    LN_RESULTCODE_ERROROVERFLOW = 9,
    LN_RESULTCODE_ERRORIO = 10,
    LN_RESULTCODE_ERRORFILE_NOT_FOUND = 11,
    LN_RESULTCODE_ERRORDIRECTORY_NOT_FOUND = 12,
    LN_RESULTCODE_ERRORINVALID_FORMAT = 13,
    LN_RESULTCODE_ERROREND_OF_STREAM = 14,
    LN_RESULTCODE_ERRORENCODING = 15,
    LN_RESULTCODE_ERRORWIN32 = 16,
    LN_RESULTCODE_ERRORCOM = 17,
} LNResultCode;
/** テクスチャのピクセルフォーマット */
typedef enum tagLNTextureFormat
{
    LN_TEXTUREFORMAT_UNKNOWN = 0,
    LN_TEXTUREFORMAT_R8G8B8A8 = 1,
    LN_TEXTUREFORMAT_R8G8B8X8 = 2,
    LN_TEXTUREFORMAT_B8G8R8A8 = 3,
    LN_TEXTUREFORMAT_B8G8R8X8 = 4,
    LN_TEXTUREFORMAT_R16G16B16A16_FLOAT = 5,
    LN_TEXTUREFORMAT_R32G32B32A32_FLOAT = 6,
    LN_TEXTUREFORMAT_R16_FLOAT = 7,
    LN_TEXTUREFORMAT_R32_FLOAT = 8,
    LN_TEXTUREFORMAT_D24S8 = 9,
} LNTextureFormat;


typedef void (*LNCollisionEventHandler)(LNHandle sender, LNHandle obj);


