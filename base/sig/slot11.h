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

#include <cassert>

// assume slotbase.h already included

namespace sig
{

// C++11 version using variadic template arguments
template< typename R, typename... P > class Slot : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P... );	// pointer to member function
	typedef R (*PFunc)( P... );				// pointer to static function
public:

	Slot()
	{
		set( 0 );
	}

	Slot( R (*pfun)( P... ) )
	{
		set( pfun );
	}

	template< class X > Slot( const X *ptr, R (X::*cpmem)( P... ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot( const X *ptr, R (X::*cpmem)( P... ) )
	{
		set( ptr, cpmem );
	}

	Slot &set( R (*pfun)( P... ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot &set( const X *ptr, R (X::*cpmem)( P... ) )
	{
		assert( ptr );
		classptr = ptr;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		// hack but works
		// note that sizeof cpmem can be <= sizeof pmem!
		assert( sizeof(func.pmem) >= sizeof(cpmem) );
		memcpy( &func.pmem, &cpmem, sizeof(cpmem) );
		return *this;
	}

	template< class X > Slot &set( const X *ptr, R (X::*cpmem)( P... ) const )
	{
		assert( ptr );
		classptr = ptr;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		// hack but works
		// note that sizeof cpmem can be <= sizeof pmem!
		memcpy( &func.pmem, &cpmem, sizeof(cpmem) );
		return *this;
	}

	// invoke slot
	inline R invoke( P... p ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)(func.pmem))( p... ) : reinterpret_cast<PFunc>(func.pfun)( p... );
	}

	// same as invoke
	inline R operator()( P... p ) const
	{
		return invoke( p... );
	}
};

}
