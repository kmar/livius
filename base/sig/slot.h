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

template< typename R> class Slot0 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)(  );		// pointer to member function
	typedef R (*PFunc)(  );				// pointer to static function
public:

	Slot0()
	{
		set( 0 );
	}

	Slot0( R (*pfun)(  ) )
	{
		set( pfun );
	}

	template< class X > Slot0( const X *ptr, R (X::*cpmem)(  ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot0( const X *ptr, R (X::*cpmem)(  ) )
	{
		set( ptr, cpmem );
	}

	Slot0 &set( R (*pfun)(  ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot0 &set( const X *ptr, R (X::*cpmem)(  ) )
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

	template< class X > Slot0 &set( const X *ptr, R (X::*cpmem)(  ) const )
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

	// invoke slot
	inline R invoke(  ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)(  ) : reinterpret_cast<PFunc>(func.pfun)(  );
	}

	// same as invoke
	inline R operator()(  ) const
	{
		return invoke(  );
	}
};

template< typename R, typename P1> class Slot1 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1 );		// pointer to member function
	typedef R (*PFunc)( P1 );				// pointer to static function
public:

	Slot1()
	{
		set( 0 );
	}

	Slot1( R (*pfun)( P1 ) )
	{
		set( pfun );
	}

	template< class X > Slot1( const X *ptr, R (X::*cpmem)( P1 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot1( const X *ptr, R (X::*cpmem)( P1 ) )
	{
		set( ptr, cpmem );
	}

	Slot1 &set( R (*pfun)( P1 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot1 &set( const X *ptr, R (X::*cpmem)( P1 ) )
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

	template< class X > Slot1 &set( const X *ptr, R (X::*cpmem)( P1 ) const )
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

	// invoke slot
	inline R invoke( P1 p1 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1 ) : reinterpret_cast<PFunc>(func.pfun)( p1 );
	}

	// same as invoke
	inline R operator()( P1 p1 ) const
	{
		return invoke( p1 );
	}
};

template< typename R, typename P1, typename P2> class Slot2 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2 );		// pointer to member function
	typedef R (*PFunc)( P1, P2 );				// pointer to static function
public:

	Slot2()
	{
		set( 0 );
	}

	Slot2( R (*pfun)( P1, P2 ) )
	{
		set( pfun );
	}

	template< class X > Slot2( const X *ptr, R (X::*cpmem)( P1, P2 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot2( const X *ptr, R (X::*cpmem)( P1, P2 ) )
	{
		set( ptr, cpmem );
	}

	Slot2 &set( R (*pfun)( P1, P2 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot2 &set( const X *ptr, R (X::*cpmem)( P1, P2 ) )
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

	template< class X > Slot2 &set( const X *ptr, R (X::*cpmem)( P1, P2 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2 ) const
	{
		return invoke( p1, p2 );
	}
};

template< typename R, typename P1, typename P2, typename P3> class Slot3 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2, P3 );		// pointer to member function
	typedef R (*PFunc)( P1, P2, P3 );				// pointer to static function
public:

	Slot3()
	{
		set( 0 );
	}

	Slot3( R (*pfun)( P1, P2, P3 ) )
	{
		set( pfun );
	}

	template< class X > Slot3( const X *ptr, R (X::*cpmem)( P1, P2, P3 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot3( const X *ptr, R (X::*cpmem)( P1, P2, P3 ) )
	{
		set( ptr, cpmem );
	}

	Slot3 &set( R (*pfun)( P1, P2, P3 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot3 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3 ) )
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

	template< class X > Slot3 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2, P3 p3 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2, p3 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2, p3 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2, P3 p3 ) const
	{
		return invoke( p1, p2, p3 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4> class Slot4 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2, P3, P4 );		// pointer to member function
	typedef R (*PFunc)( P1, P2, P3, P4 );				// pointer to static function
public:

	Slot4()
	{
		set( 0 );
	}

	Slot4( R (*pfun)( P1, P2, P3, P4 ) )
	{
		set( pfun );
	}

	template< class X > Slot4( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot4( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ) )
	{
		set( ptr, cpmem );
	}

	Slot4 &set( R (*pfun)( P1, P2, P3, P4 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot4 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ) )
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

	template< class X > Slot4 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2, P3 p3, P4 p4 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2, p3, p4 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2, p3, p4 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4 ) const
	{
		return invoke( p1, p2, p3, p4 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5> class Slot5 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2, P3, P4, P5 );		// pointer to member function
	typedef R (*PFunc)( P1, P2, P3, P4, P5 );				// pointer to static function
public:

	Slot5()
	{
		set( 0 );
	}

	Slot5( R (*pfun)( P1, P2, P3, P4, P5 ) )
	{
		set( pfun );
	}

	template< class X > Slot5( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot5( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ) )
	{
		set( ptr, cpmem );
	}

	Slot5 &set( R (*pfun)( P1, P2, P3, P4, P5 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot5 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ) )
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

	template< class X > Slot5 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2, p3, p4, p5 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2, p3, p4, p5 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ) const
	{
		return invoke( p1, p2, p3, p4, p5 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6> class Slot6 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2, P3, P4, P5, P6 );		// pointer to member function
	typedef R (*PFunc)( P1, P2, P3, P4, P5, P6 );				// pointer to static function
public:

	Slot6()
	{
		set( 0 );
	}

	Slot6( R (*pfun)( P1, P2, P3, P4, P5, P6 ) )
	{
		set( pfun );
	}

	template< class X > Slot6( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot6( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ) )
	{
		set( ptr, cpmem );
	}

	Slot6 &set( R (*pfun)( P1, P2, P3, P4, P5, P6 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot6 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ) )
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

	template< class X > Slot6 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2, p3, p4, p5, p6 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2, p3, p4, p5, p6 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ) const
	{
		return invoke( p1, p2, p3, p4, p5, p6 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7> class Slot7 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2, P3, P4, P5, P6, P7 );		// pointer to member function
	typedef R (*PFunc)( P1, P2, P3, P4, P5, P6, P7 );				// pointer to static function
public:

	Slot7()
	{
		set( 0 );
	}

	Slot7( R (*pfun)( P1, P2, P3, P4, P5, P6, P7 ) )
	{
		set( pfun );
	}

	template< class X > Slot7( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot7( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ) )
	{
		set( ptr, cpmem );
	}

	Slot7 &set( R (*pfun)( P1, P2, P3, P4, P5, P6, P7 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot7 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ) )
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

	template< class X > Slot7 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2, p3, p4, p5, p6, p7 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2, p3, p4, p5, p6, p7 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ) const
	{
		return invoke( p1, p2, p3, p4, p5, p6, p7 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8> class Slot8 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2, P3, P4, P5, P6, P7, P8 );		// pointer to member function
	typedef R (*PFunc)( P1, P2, P3, P4, P5, P6, P7, P8 );				// pointer to static function
public:

	Slot8()
	{
		set( 0 );
	}

	Slot8( R (*pfun)( P1, P2, P3, P4, P5, P6, P7, P8 ) )
	{
		set( pfun );
	}

	template< class X > Slot8( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot8( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ) )
	{
		set( ptr, cpmem );
	}

	Slot8 &set( R (*pfun)( P1, P2, P3, P4, P5, P6, P7, P8 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot8 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ) )
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

	template< class X > Slot8 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2, p3, p4, p5, p6, p7, p8 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2, p3, p4, p5, p6, p7, p8 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ) const
	{
		return invoke( p1, p2, p3, p4, p5, p6, p7, p8 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9> class Slot9 : public SlotBase
{
private:
	typedef R (DummyClass::*PMem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 );		// pointer to member function
	typedef R (*PFunc)( P1, P2, P3, P4, P5, P6, P7, P8, P9 );				// pointer to static function
public:

	Slot9()
	{
		set( 0 );
	}

	Slot9( R (*pfun)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) )
	{
		set( pfun );
	}

	template< class X > Slot9( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const )
	{
		set( ptr, cpmem );
	}

	template< class X > Slot9( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) )
	{
		set( ptr, cpmem );
	}

	Slot9 &set( R (*pfun)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) )
	{
		classptr = 0;
		// avoid uninitialized memory access (this is required for relational operators too)
		memset( &func.pmem, 0, sizeof(func.pmem) );
		assert( sizeof(func.pfun) == sizeof(pfun) );
		memcpy( &func.pfun, &pfun, sizeof(pfun) );
		return *this;
	}

	template< class X > Slot9 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) )
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

	template< class X > Slot9 &set( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const )
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

	// invoke slot
	inline R invoke( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ) const	{
		return classptr ? ( ((DummyClass*)classptr)->*(PMem &)func.pmem)( p1, p2, p3, p4, p5, p6, p7, p8, p9 ) : reinterpret_cast<PFunc>(func.pfun)( p1, p2, p3, p4, p5, p6, p7, p8, p9 );
	}

	// same as invoke
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ) const
	{
		return invoke( p1, p2, p3, p4, p5, p6, p7, p8, p9 );
	}
};

template< typename R, typename P1=void, typename P2=void, typename P3=void, typename P4=void, typename P5=void, typename P6=void, typename P7=void, typename P8=void, typename P9=void >
	class Slot : public Slot9< R, P1, P2, P3, P4, P5, P6, P7, P8, P9 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 >
	class Slot< R, P1, P2, P3, P4, P5, P6, P7, P8, void > : public Slot8< R, P1, P2, P3, P4, P5, P6, P7, P8 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 >
	class Slot< R, P1, P2, P3, P4, P5, P6, P7, void, void > : public Slot7< R, P1, P2, P3, P4, P5, P6, P7 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 >
	class Slot< R, P1, P2, P3, P4, P5, P6, void, void, void > : public Slot6< R, P1, P2, P3, P4, P5, P6 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5 >
	class Slot< R, P1, P2, P3, P4, P5, void, void, void, void > : public Slot5< R, P1, P2, P3, P4, P5 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4 >
	class Slot< R, P1, P2, P3, P4, void, void, void, void, void > : public Slot4< R, P1, P2, P3, P4 > {};
template< typename R, typename P1, typename P2, typename P3 >
	class Slot< R, P1, P2, P3, void, void, void, void, void, void > : public Slot3< R, P1, P2, P3 > {};
template< typename R, typename P1, typename P2 >
	class Slot< R, P1, P2, void, void, void, void, void, void, void > : public Slot2< R, P1, P2 > {};
template< typename R, typename P1 >
	class Slot< R, P1, void, void, void, void, void, void, void, void > : public Slot1< R, P1 > {};
template< typename R >
	class Slot< R, void, void, void, void, void, void, void, void, void > : public Slot0< R > {};

}
