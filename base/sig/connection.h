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

// assume signalbase.h already included

namespace sig
{

class Connection : public SlotBase
{
	SignalBase *signalRef;					// refptr to owner
public:
	inline Connection() : signalRef(0) {}
	Connection( const SlotBase &sl, SignalBase *sigRef ) : SlotBase(sl), signalRef(sigRef) {}

	// connected to signal?
	// note: this won't work if connection was already disconnected manually
	bool connected() const
	{
		return signalRef != 0;
	}

	// disconned from signal
	bool disconnect()
	{
		if ( signalRef && signalRef->disconnectSlot( *this ) )
		{
			signalRef = 0;
			return 1;
		}
		return 0;
	}

	// lt test to allow stl sorting
	bool operator <( const Connection &o ) const
	{
		if ( signalRef != o.signalRef )
			return signalRef < o.signalRef;
		return SlotBase::operator <(o);
	}

	// equality test
	bool operator ==( const Connection &o ) const
	{
		return signalRef == o.signalRef && SlotBase::operator ==(o);
	}
	// inequality test
	bool operator !=( const Connection &o ) const
	{
		return !(*this == o);
	}
};

}
