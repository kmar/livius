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
#include "slot11.h"
#include <vector>

namespace sig
{

// C++11 version using variadic template arguments
template< typename R, typename... P > class Signal : public SignalBase
{
private:
	typedef Slot< R, P... > Callback;
public:
	Connection connect( const Callback &cbk, bool disconn = 0 )	
	{
		if ( disconn )
		{
			disconnect( cbk );
			return Connection( SlotBase(), 0 );
		}
		return Connection(connectSlot(cbk), this);
	}
	Connection connect( Signal &s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( &s, &Signal::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( &s, &Signal::emitSignal );
	}
	Connection connect( const Signal *s, bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( s, &Signal::emitSignal );
			return Connection( SlotBase(), 0 );
		}
		return connect( s, &Signal::emitSignal );
	}
	Connection connect( R (*pfun)( P... ), bool disconn = 0 )
	{
		if ( disconn )
		{
			disconnect( pfun );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(pfun) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P... ) const, bool disconn = 0 )
	{
		assert( ptr );
		if ( disconn )
		{
			disconnect( ptr, cpmem );
			return Connection( SlotBase(), 0 );
		}
		return connect( Callback(ptr, cpmem) );
	}
	template< class X > Connection connect( const X *ptr, R (X::*cpmem)( P... ), bool disconn = 0 )
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
	bool disconnect( const SlotBase &sl )
	{
		return disconnectSlot( sl );
	}
	bool disconnect( R (*cpfun)( P... ) )
	{
		return disconnect( Callback(cpfun) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P... ) const )
	{
		return disconnect( Callback(ptr, cpmem) );
	}
	template< class X > bool disconnect( const X *ptr, R (X::*cpmem)( P... ) )
	{
		return disconnect( Callback(ptr, cpmem) );
	}

	// emit signal
	// return value: if a slot returns a value, the return value of the last connected slot is returned
	R emitSignal( P... p ) const
	{
		if ( enabled() )
			for ( typename Connections::const_iterator ci = connections.begin(); ci != connections.end(); )
			{
				assert( *ci );
				typename Connections::const_iterator cin = ci;
				if ( ++cin == connections.end() )
					return reinterpret_cast<const Callback &>(*ci).invoke( p... );
				else
					reinterpret_cast<const Callback &>(*ci).invoke( p... );
				ci = cin;
			}
		return R();		// make sure we return at least something here (and compiler will be very happy too)
	}
	// emitSignal alias
	inline R operator()( P... p ) const
	{
		return emitSignal( p... );
	}
};

}
