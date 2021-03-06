﻿
#include "../Internal.h"
#if defined(LN_THREAD_WIN32)
#else
#include <pthread.h>
#endif

#include <Lumino/Threading/AtomicCounter.h>

LN_NAMESPACE_BEGIN

#if defined(LN_THREAD_WIN32)
//==============================================================================
// Atomic (Win32)
//==============================================================================
//------------------------------------------------------------------------------
Atomic::Atomic(int32_t initialValue)
	: m_value(initialValue)
{
}

//------------------------------------------------------------------------------
Atomic::~Atomic()
{
}

//------------------------------------------------------------------------------
int32_t Atomic::get() const
{
	return ::InterlockedExchangeAdd( const_cast< volatile LONG* >( &m_value ), 0 );
}

//------------------------------------------------------------------------------
void Atomic::set( int32_t nValue )
{
    ::InterlockedExchange( &m_value, nValue );
}

//------------------------------------------------------------------------------
int32_t Atomic::increment()
{
    return ::InterlockedIncrement( &m_value );
}

//------------------------------------------------------------------------------
int32_t Atomic::decrement()
{
    return ::InterlockedDecrement( &m_value );
}

#else
//==============================================================================
// Atomic (pthread)
//==============================================================================
// __sync_fetch_and_add でもできるけど、環境依存ある。
// http://fallabs.com/blog-ja/promenade.cgi?id=68
// とりあえず今は確実なミューテックスで。
//------------------------------------------------------------------------------
Atomic::Atomic( int32_t value )
    : m_value( value )
{
    pthread_mutex_init( &m_mutex, NULL );
}

//------------------------------------------------------------------------------
Atomic::~Atomic()
{
    pthread_mutex_destroy( &m_mutex );
}

//------------------------------------------------------------------------------
int32_t Atomic::get() const
{
	pthread_mutex_lock( &m_mutex );
    int32_t v = m_value;
    pthread_mutex_unlock( &m_mutex );
    return v;
}

//------------------------------------------------------------------------------
void Atomic::set( int32_t value_ )
{
    pthread_mutex_lock( &m_mutex );
    m_value = value_;
    pthread_mutex_unlock( &m_mutex );
}

//------------------------------------------------------------------------------
int32_t Atomic::increment()
{
    pthread_mutex_lock( &m_mutex );
    ++m_value;
    int32_t v = m_value;
    pthread_mutex_unlock( &m_mutex );
    return v;
}

//------------------------------------------------------------------------------
int32_t Atomic::decrement()
{
    pthread_mutex_lock( &m_mutex );
    --m_value;
    int32_t v = m_value;
    pthread_mutex_unlock( &m_mutex );
    return v;
}

#endif

LN_NAMESPACE_END
