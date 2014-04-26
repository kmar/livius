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

#include "signalbase.h"
#include "connection.h"
#include "slot.h"
#include <vector>

namespace sig
{

template< typename R > class Signal0 : public SignalBase
{
private:
	typedef Slot0< R> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal0 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal0::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal0::emitSignal );
	}
	Connection connect( const Signal0 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal0::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal0::emitSignal );
	}
	Connection connect( R (*pfun)(  ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)(  ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)(  ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)(  ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)(  ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)(  ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal(  ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke(  );
				else
					reinterpret_cast< const Callback &>(*ci).invoke(  );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()(  ) const
	{
		return emitSignal(  );
	}
};

template< typename R, typename P1 > class Signal1 : public SignalBase
{
private:
	typedef Slot1< R, P1> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal1 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal1::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal1::emitSignal );
	}
	Connection connect( const Signal1 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal1::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal1::emitSignal );
	}
	Connection connect( R (*pfun)( P1 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1 ) const
	{
		return emitSignal( p1 );
	}
};

template< typename R, typename P1, typename P2 > class Signal2 : public SignalBase
{
private:
	typedef Slot2< R, P1, P2> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal2 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal2::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal2::emitSignal );
	}
	Connection connect( const Signal2 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal2::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal2::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2 ) const
	{
		return emitSignal( p1, p2 );
	}
};

template< typename R, typename P1, typename P2, typename P3 > class Signal3 : public SignalBase
{
private:
	typedef Slot3< R, P1, P2, P3> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal3 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal3::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal3::emitSignal );
	}
	Connection connect( const Signal3 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal3::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal3::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2, P3 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2, P3 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2, P3 p3 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2, P3 p3 ) const
	{
		return emitSignal( p1, p2, p3 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4 > class Signal4 : public SignalBase
{
private:
	typedef Slot4< R, P1, P2, P3, P4> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal4 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal4::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal4::emitSignal );
	}
	Connection connect( const Signal4 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal4::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal4::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2, P3, P4 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2, P3, P4 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2, P3 p3, P4 p4 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4 ) const
	{
		return emitSignal( p1, p2, p3, p4 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5 > class Signal5 : public SignalBase
{
private:
	typedef Slot5< R, P1, P2, P3, P4, P5> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal5 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal5::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal5::emitSignal );
	}
	Connection connect( const Signal5 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal5::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal5::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2, P3, P4, P5 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2, P3, P4, P5 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 ) const
	{
		return emitSignal( p1, p2, p3, p4, p5 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > class Signal6 : public SignalBase
{
private:
	typedef Slot6< R, P1, P2, P3, P4, P5, P6> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal6 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal6::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal6::emitSignal );
	}
	Connection connect( const Signal6 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal6::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal6::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2, P3, P4, P5, P6 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2, P3, P4, P5, P6 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 ) const
	{
		return emitSignal( p1, p2, p3, p4, p5, p6 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > class Signal7 : public SignalBase
{
private:
	typedef Slot7< R, P1, P2, P3, P4, P5, P6, P7> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal7 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal7::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal7::emitSignal );
	}
	Connection connect( const Signal7 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal7::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal7::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2, P3, P4, P5, P6, P7 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2, P3, P4, P5, P6, P7 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6, p7 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6, p7 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 ) const
	{
		return emitSignal( p1, p2, p3, p4, p5, p6, p7 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > class Signal8 : public SignalBase
{
private:
	typedef Slot8< R, P1, P2, P3, P4, P5, P6, P7, P8> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal8 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal8::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal8::emitSignal );
	}
	Connection connect( const Signal8 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal8::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal8::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2, P3, P4, P5, P6, P7, P8 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2, P3, P4, P5, P6, P7, P8 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6, p7, p8 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6, p7, p8 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ) const
	{
		return emitSignal( p1, p2, p3, p4, p5, p6, p7, p8 );
	}
};

template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > class Signal9 : public SignalBase
{
private:
	typedef Slot9< R, P1, P2, P3, P4, P5, P6, P7, P8, P9> Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect(cbk);
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal9 &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal9::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal9::emitSignal );
	}
	Connection connect( const Signal9 *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal9::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal9::emitSignal );
	}
	Connection connect( R (*pfun)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ), bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	// returns 1 if successfully disconnected
	bool disconnect( const SlotBase &slot )
	{
		return disconnectSlot( slot );
	}
	bool disconnect( R (*cpfun)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6, p7, p8, p9 );
				else
					reinterpret_cast< const Callback &>(*ci).invoke( p1, p2, p3, p4, p5, p6, p7, p8, p9 );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9 ) const
	{
		return emitSignal( p1, p2, p3, p4, p5, p6, p7, p8, p9 );
	}
};

template< typename R, typename P1=void, typename P2=void, typename P3=void, typename P4=void, typename P5=void, typename P6=void, typename P7=void, typename P8=void, typename P9=void >
	class Signal : public Signal9< R, P1, P2, P3, P4, P5, P6, P7, P8, P9 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 >
	class Signal< R, P1, P2, P3, P4, P5, P6, P7, P8, void > : public Signal8< R, P1, P2, P3, P4, P5, P6, P7, P8 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 >
	class Signal< R, P1, P2, P3, P4, P5, P6, P7, void, void > : public Signal7< R, P1, P2, P3, P4, P5, P6, P7 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 >
	class Signal< R, P1, P2, P3, P4, P5, P6, void, void, void > : public Signal6< R, P1, P2, P3, P4, P5, P6 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4, typename P5 >
	class Signal< R, P1, P2, P3, P4, P5, void, void, void, void > : public Signal5< R, P1, P2, P3, P4, P5 > {};
template< typename R, typename P1, typename P2, typename P3, typename P4 >
	class Signal< R, P1, P2, P3, P4, void, void, void, void, void > : public Signal4< R, P1, P2, P3, P4 > {};
template< typename R, typename P1, typename P2, typename P3 >
	class Signal< R, P1, P2, P3, void, void, void, void, void, void > : public Signal3< R, P1, P2, P3 > {};
template< typename R, typename P1, typename P2 >
	class Signal< R, P1, P2, void, void, void, void, void, void, void > : public Signal2< R, P1, P2 > {};
template< typename R, typename P1 >
	class Signal< R, P1, void, void, void, void, void, void, void, void > : public Signal1< R, P1 > {};
template< typename R >
	class Signal< R, void, void, void, void, void, void, void, void, void > : public Signal0< R > {};

}
