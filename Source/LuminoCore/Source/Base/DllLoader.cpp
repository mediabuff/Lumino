﻿
#include "../Internal.h"
#ifdef _WIN32
#else
	#include <dlfcn.h>	// Ubuntu
#endif
#include <Lumino/Base/DllLoader.h>
#include <Lumino/IO/PathName.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// DllLoader
//==============================================================================

static void* LoadDLL(const Char* filePath)
{
#ifdef _WIN32
	return ::LoadLibrary(filePath);
#else
	detail::GenericStaticallyLocalPath<char> localPath(filePath, StringTraits::tcslen(filePath));
	return dlopen(localPath.c_str(), RTLD_LAZY);
#endif
}

static void UnloadDLL(void* module)
{
#ifdef _WIN32
	::FreeLibrary((HMODULE)module);
#else
	dlclose(module);
#endif
}

static void* GetProcAddr(void* module, const char* procName)
{
#ifdef _WIN32
	return ::GetProcAddress((HMODULE)module, procName);
#else
	return dlsym(module, procName);
#endif
}

//------------------------------------------------------------------------------
DllLoader::DllLoader()
	: m_module(NULL)
{
}

//------------------------------------------------------------------------------
DllLoader::DllLoader(const Char* filePath)
	: m_module(NULL)
{
	load(filePath);
}

//------------------------------------------------------------------------------
DllLoader::~DllLoader()
{
	unload();
}

//------------------------------------------------------------------------------
bool DllLoader::exists(const Char* filePath)
{
	// ロードできるかでチェック
	void* module = LoadDLL(filePath);
	if (module != NULL)
	{
		UnloadDLL(module);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void DllLoader::load(const Char* filePath)
{
	unload();
	m_module = LoadDLL(filePath);
	LN_ENSURE_FILE_NOT_FOUND(m_module, filePath);
}

//------------------------------------------------------------------------------
bool DllLoader::tryLoad(const Char* filePath)
{
	unload();
	m_module = LoadDLL(filePath);
	return m_module != NULL;
}

//------------------------------------------------------------------------------
void DllLoader::unload()
{
	if (m_module != NULL)
	{
		UnloadDLL(m_module);
		m_module = NULL;
	}
}

//------------------------------------------------------------------------------
void* DllLoader::getProcAddress(const char* procName)
{
	LN_REQUIRE(m_module);
	void* proc = GetProcAddr(m_module, procName);
	LN_ENSURE(proc);
	return proc;
}

LN_NAMESPACE_END
