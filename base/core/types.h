#pragma once

#ifndef _MSC_VER
#	include <stdint.h>			// uintptr_t, ...
#else
typedef signed __int8		int8_t;
typedef unsigned __int8		uint8_t;
typedef signed __int16		int16_t;
typedef unsigned __int16	uint16_t;
typedef signed __int32		int32_t;
typedef unsigned __int32	uint32_t;
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;
#endif

#include <stddef.h>			// size_t, ...

namespace core
{

// probably more useful aliases
typedef uint8_t				b8;
typedef int8_t				i8;
typedef uint8_t				u8;
typedef int16_t				i16;
typedef uint16_t			u16;
typedef int32_t				i32;
typedef uint32_t			u32;
typedef int64_t				i64;
typedef uint64_t			u64;

typedef unsigned int		uint;

template < class T > T abs( const T &x )
{
	return x < 0 ? -x : x;
}

}

// "thanks" Qt :)
#ifdef QT_NO_DEBUG
#   define NDEBUG
#endif

#include <cassert>
