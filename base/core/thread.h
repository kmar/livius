#pragma once

#include "types.h"

namespace core
{

// platform-independent Threads

class Mutex
{
	Mutex( const Mutex & )
	{
		assert( 0 && "Mutex cannot be copied!" );
	}
protected:
	void *handle;		// system-specific mutex handle
public:
	Mutex( int enabled = 1 );
	~Mutex();

	void lock();
	void unlock();
};

class MutexLock
{
protected:
	Mutex *mref;		// mutex refptr
public:
	MutexLock( Mutex &m, bool nolock = 0 );
	~MutexLock();
};

class Event
{
	Event( const Event & )
	{
		assert( 0 && "Event cannot be copied!" );
	}
protected:
	void *handle, *handle2;
	bool autoReset;
	volatile bool flag;
public:
	Event( bool autoReset_ = 1 );
	~Event();

	bool wait( int ms = -1 );
	void reset();
	void signal();
};

// thread is not run/created until the first call to resume()
class Thread
{
protected:
	void *thrParam;
	void *handle;					// sys-specific handle
	volatile bool shouldTerminate;	// thread should terminate flag
	volatile bool killFlag;			// killing flag
public:
	Thread( void *param = 0 );
	// never implement dtor, implement destroy() instead
	virtual ~Thread();

	// use this instead of dtor!
	virtual void destroy();

	// run thread (first time)
	virtual bool run();

	// main worker proc
	// MUST be overridden
	virtual void work() = 0;

	// wait for thread to terminate
	void wait();

	// kill thread (DON'T USE DELETE!!!)
	// also waits for thread to terminate
	void kill();

	// sets thread priority: 0 = normal, -1 = below normal, +1 = above normal
	bool setPriority( int prio );

	// sleep in ms
	static void sleep( int ms );

	// returns current thread id
	static void *current();
};

}
