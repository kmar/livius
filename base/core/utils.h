#pragma once

#include <cstddef>

// various utility functions that didn't fit elsewhere

namespace core
{

// returns true if input is power of two
bool isPow2( size_t sz );
// round size to nearest power of two
bool roundPow2( size_t &sz );
// align pointer
void *alignPtr( void *ptr, size_t align );
// simple unsafe string copy (doesn't copy null terminator!)
char *scpy( char *dst, const char *src );

}
