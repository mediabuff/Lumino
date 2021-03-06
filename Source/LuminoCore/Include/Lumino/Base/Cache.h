﻿
#pragma once
#include <map>
#include "String.h"
#include "RefObject.h"
#include "../IO/PathName.h"
#include "../Threading/Mutex.h"

LN_NAMESPACE_BEGIN
class CacheManager;

/**
	@brief	キャッシュの検索キーです。
*/
class CacheKey
{
public:
	static const CacheKey Null;

public:
	CacheKey();
	explicit CacheKey(uint64_t hashCode);
	explicit CacheKey(const Char* str);
	explicit CacheKey(const String& str);
	explicit CacheKey(const StringRef& str);

	bool operator == (const CacheKey& key) const;
	bool operator < (const CacheKey& key) const;

	bool isNull() const;

private:

	enum Type
	{
		Type_Null = 0,
		Type_HashCode,
		Type_String,
	};

	Type		m_keyType;
	uint64_t	m_hashCode;
	String		m_string;
};

/**
	@brief		キャッシュ可能なオブジェクトのインターフェイスです。
	@details	ICacheObject は RefObject とセットで使用します。
				両方を継承したクラスに LN_CACHE_OBJECT_DECL マクロで必要な機能を実装します。
				ICacheObject は参照カウントが 0 になったとき、delete はされずキャッシュに入れられます。
*/
class ICacheObject
{
protected:
	friend class CacheManager;
	struct CacheObjectInfo
	{
		CacheManager*	manager;
		CacheKey		key;
		size_t			cacheMemorySize;
		bool			inCacheList;

		CacheObjectInfo();
		~CacheObjectInfo();
	};

public:
	ICacheObject() {}

protected:
	virtual ~ICacheObject() {}
	virtual CacheObjectInfo&	getCacheObjectInfo() = 0;

public:
	virtual int32_t	addRef() = 0;
	virtual int32_t	release() = 0;
};

/// ICacheObject の実装ユーティリティ
#define LN_CACHE_OBJECT_DECL \
protected: \
	CacheObjectInfo m_cacheObjectInfo; \
public: \
	virtual CacheObjectInfo& getCacheObjectInfo() override { return m_cacheObjectInfo; } \
	virtual int32_t addRef() override { return RefObject::addRef(); } \
	virtual int32_t release() override \
	{ \
		if (m_cacheObjectInfo.manager == NULL/* || m_cacheObjectInfo.mIsStockObject*/) \
		{ \
			return RefObject::release(); \
		} \
		int32_t count = m_referenceCount.decrement(); \
		LN_ASSERT(count >= 0); \
		if (count == 0) \
		{ \
			m_cacheObjectInfo.manager->addCacheUnusedList(this); \
		} \
		return count; \
	}

/**
	@brief	キャッシュオブジェクトの管理クラスです。
*/
class CacheManager
	: public RefObject
{
public:
	/**
		@brief		初期化
		@param[in]	capacityCount	: キャッシュに保持できる最大オブジェクト数
		@param[in]	maxMemorySize	: キャッシュに保持できる最大メモリ量 (byte単位。0 の場合はメモリ量を考慮しない)
	*/
	CacheManager(int capacityCount, size_t maxMemorySize);

public:

	/**
		@brief	終了処理 (これ以降キャッシュにオブジェクトを追加しないようにする)
	*/
	void finalizeCache();

	/**
		@brief	オブジェクトをキャッシュ管理に追加します。
	*/
	void registerCacheObject(const CacheKey& key, ICacheObject* obj);

	/**
		@brief		キーに一致するオブジェクトを検索します。
		@details	参照カウントをインクリメントして返します。
	*/
	ICacheObject* findObjectAddRef(const CacheKey& key);
	
	/**
		@brief	キャッシュをクリアします。
	*/
	void clearCache();

public:
	void addCacheUnusedList(ICacheObject* obj);
	void deleteCachedObject(ICacheObject* obj);

private:
	virtual ~CacheManager();

private:
	friend class ICacheObject;
	class CacheUnusedList;
	typedef std::map<CacheKey, ICacheObject*>	CacheUsingMap;
	typedef std::pair<CacheKey, ICacheObject*>	CacheUsingPair;

	CacheUnusedList*	m_cacheUnusedList;
	CacheUsingMap		m_cacheUsingMap;
	Mutex				m_mutex;
};

LN_NAMESPACE_END
