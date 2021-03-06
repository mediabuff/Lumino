﻿#include "../Internal.h"
#if defined(LN_OS_WIN32)
#else
	#include <sys/time.h>
#endif

#include <Lumino/Base/ElapsedTimer.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// ElapsedTimer
//==============================================================================

//------------------------------------------------------------------------------
ElapsedTimer::ElapsedTimer()
	: m_start(0)
#if defined(LN_OS_WIN32)
	, m_freq(0)
#endif
{
}

//------------------------------------------------------------------------------
ElapsedTimer::~ElapsedTimer()
{
}

#if defined(LN_OS_WIN32)

static uint64_t TicksToNanoseconds(uint64_t ticks, uint64_t freq) LN_NOEXCEPT
{
	uint64_t seconds = ticks / freq;
	uint64_t nanoSeconds = (ticks - seconds * freq) * 1000000000 / freq;
	return seconds * 1000000000 + nanoSeconds;
}

//------------------------------------------------------------------------------
void ElapsedTimer::start()
{
	LARGE_INTEGER freq;
	LARGE_INTEGER counter;
	::QueryPerformanceFrequency(&freq);
	::QueryPerformanceCounter(&counter);
	m_freq = freq.QuadPart;
	m_start = counter.QuadPart;
}

//------------------------------------------------------------------------------
uint64_t ElapsedTimer::getElapsed() const LN_NOEXCEPT
{
	if (m_freq == 0) return 0;
	LARGE_INTEGER counter;
	::QueryPerformanceCounter(&counter);
	return TicksToNanoseconds(counter.QuadPart - m_start, m_freq);
}

#else

//------------------------------------------------------------------------------
void ElapsedTimer::start()
{
	timeval t;
	gettimeofday(&t, NULL);	// 第2引数は廃止予定 http://linuxjm.osdn.jp/html/LDP_man-pages/man2/gettimeofday.2.html
	m_start = (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_usec;
}


//------------------------------------------------------------------------------
uint64_t ElapsedTimer::getElapsed() const LN_NOEXCEPT
{
	timeval t;
	gettimeofday(&t, NULL);
	uint64_t ct = (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_usec;
	return (ct - m_start);
}

#endif

LN_NAMESPACE_END
