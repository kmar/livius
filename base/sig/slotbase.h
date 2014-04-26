/*
sig - a lightweight, typesafe C++ signal/slot library

Copyright (c) 2013 Martin Sedlak (mar)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#pragma once

#include <memory.h>

namespace sig
{

class SlotBase
{
protected:
	class DummyClass;
	typedef int (DummyClass::*BPMem)();		// pointer to member function
	typedef int (*BPFunc)();				// pointer to static function
	union UFunc								// don't waste space
	{
		BPMem pmem;
		BPFunc pfun;
	} func;
	const void *classptr;					// class pointer
public:
	// null test
	inline operator bool() const
	{
		return classptr ? func.pmem != 0 : func.pfun != 0;
	}

	// lt test to allow stl sorting
	bool operator <( const SlotBase &o ) const
	{
		if ( classptr != o.classptr )
			return classptr < o.classptr;
		return memcmp( &func.pmem, &o.func.pmem, sizeof(func.pmem) ) < 0;
	}

	// equality test
	bool operator ==( const SlotBase &o ) const
	{
		return classptr == o.classptr && memcmp( &func.pmem, &o.func.pmem, sizeof(func.pmem) ) == 0;
	}
	// inequality test
	bool operator !=( const SlotBase &o ) const
	{
		return !(*this == o );
	}
};

}
