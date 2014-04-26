#include "utils.h"
#include <assert.h>
#ifndef _MSC_VER
#include <stdint.h>
#endif

namespace core
{

// returns true if input is power of two
bool isPow2( size_t sz )
{
	size_t tmp = sz;
	while ( tmp < sz )
	{
		size_t otmp = tmp;
		tmp <<= 1;
		if ( tmp < otmp )
			return 0;			// overflow
	}
	return tmp == sz;
}

// round size to nearest power of two
bool roundPow2( size_t &sz )
{
	size_t tmp = 1;
	while ( tmp < sz )
	{
		size_t otmp = tmp;
		tmp <<= 1;
		if ( tmp < otmp )
			return 0;			// overflow
	}
	sz = tmp;
	return 1;
}

// align pointer
void *alignPtr( void *ptr, size_t align )
{
	assert( isPow2(align) );
	return reinterpret_cast<void *>( ((uintptr_t)ptr + align-1) & ~(uintptr_t)(align-1) );
}

// simple unsafe string copy (doesn't copy null terminator!)
char *scpy( char *dst, const char *src )
{
	while (*src)
		*dst++ = *src++;
	return dst;
}

}
