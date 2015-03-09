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

#include "slotbase.h"
#include <cassert>
#include <vector>

namespace sig
{

class SignalBase
{
protected:
	typedef std::vector< SlotBase > Connections;
	Connections connections;				// list of connected slots
	bool enableFlag;						// enabled flag (disable signal doesn't emit anything)

	SlotBase connectSlot( const SlotBase &conn )
	{
		if ( !conn )
			return conn;					// don't connect empty slots
		for ( Connections::const_iterator ci = connections.begin(); ci != connections.end(); ci++ )
			assert( *ci != conn );			// make sure no slot is connected twice
		connections.push_back( conn );
		return conn;
	}
public:
	inline SignalBase() : enableFlag(1) {}

	// operator bool: anything connected?
	inline operator bool() const
	{
		return !connections.empty();
	}

	// disconnect slot
	bool disconnectSlot( const SlotBase &conn )
	{
		// don't want to depend on <algorithm> here
		// reverse iterators suck
		for ( Connections::reverse_iterator it = connections.rbegin(); it != connections.rend(); it++ )
			if ( *it == conn )
			{
				connections.erase( (it+1).base() );
				// clean up
				if ( connections.empty() )
					disconnectAll();
				return 1;
			}
		return 0;
	}

	// disconnect all
	void disconnectAll()
	{
		std::vector< SlotBase > v;	// make gcc happy
		connections.swap( v );		// swap trick to reset capacity to 0
	}

	// is enabled?
	inline bool enabled() const
	{
		return enableFlag;
	}

	// enable signal (by default all signals are enabled)
	inline void enable( bool flag = 1 )
	{
		enableFlag = flag;
	}

	inline void disable()
	{
		enableFlag = 0;
	}
};

}
