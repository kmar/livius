#include "thread.h"

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#include <time.h>
#else
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#endif

namespace core
{

// Mutex

Mutex::Mutex( int enabled ) : handle(0)
{
	if ( !enabled )
		return;
#ifdef _WIN32
	handle = new CRITICAL_SECTION;
	InitializeCriticalSection( (LPCRITICAL_SECTION)handle );
#else
	handle = new pthread_mutex_t;
	pthread_mutex_init( (pthread_mutex_t *)handle, 0 );
#endif
}

Mutex::~Mutex()
{
	if ( handle )
	{
#ifdef _WIN32
		DeleteCriticalSection( (LPCRITICAL_SECTION)handle );
		delete (CRITICAL_SECTION *)handle;
#else
		pthread_mutex_destroy( (pthread_mutex_t *)handle );
		delete (pthread_mutex_t *)handle;
#endif
	}
}

void Mutex::lock()
{
	if ( !handle )
		return;
#ifdef _WIN32
	EnterCriticalSection( (LPCRITICAL_SECTION)handle );
#else
	pthread_mutex_lock( (pthread_mutex_t *)handle );
#endif
}

void Mutex::unlock()
{
	if ( !handle )
		return;
#ifdef _WIN32
	LeaveCriticalSection( (LPCRITICAL_SECTION)handle );
#else
	pthread_mutex_unlock( (pthread_mutex_t *)handle );
#endif
}

// MutexLock

MutexLock::MutexLock( Mutex &m, bool nolock ) : mref( &m )
{
	if ( nolock )
		mref = 0;
	else
		mref->lock();
}

MutexLock::~MutexLock()
{
	if ( mref )
		mref->unlock();
}

// Event

Event::Event(bool autoReset_) : handle(0), handle2(0), autoReset( autoReset_ ), flag(0)
{
#ifdef _WIN32
	handle = CreateEvent( 0, autoReset ? FALSE : TRUE, FALSE, 0);
#else
	handle = new pthread_cond_t;
	pthread_cond_init((pthread_cond_t *)handle, 0);
	handle2 = new pthread_mutex_t;
	pthread_mutex_init( (pthread_mutex_t *)handle2, 0 );
#endif
}

Event::~Event()
{
#ifdef _WIN32
	if ( handle != 0 )
		CloseHandle( (HANDLE)handle );

#else
	pthread_cond_destroy((pthread_cond_t *)handle);
	delete (pthread_cond_t *)handle;
	pthread_mutex_destroy( (pthread_mutex_t *)handle2 );
	delete (pthread_mutex_t *)handle2;
#endif
}

bool Event::wait( int ms )
{
#ifdef _WIN32
	if ( handle != 0 )
	{
		return WaitForSingleObject( (HANDLE)handle,
			ms == -1 ? INFINITE : ms ) == WAIT_OBJECT_0;
	}
	return 0;
#else
	int res = 0;
	if ( ms == -1 )
	{
		pthread_mutex_lock( (pthread_mutex_t *)handle2 );
		while ( !flag && (res = pthread_cond_wait((pthread_cond_t *)handle, (pthread_mutex_t *)handle2)) == 0 )
			;
		if ( autoReset )
			flag = 0 ;
		pthread_mutex_unlock( (pthread_mutex_t *)handle2 );
	}
	else
	{
		struct timeval tv;
		struct timespec ts;
		gettimeofday(&tv, 0);
		tv.tv_sec += ms / 1000;
		tv.tv_usec += (ms % 1000) * 1000;
		// handle overflow
		while ( tv.tv_usec >= 1000000 )
		{
			tv.tv_usec -= 1000000;
			tv.tv_sec++;
		}
		ts.tv_sec = tv.tv_sec;
		ts.tv_nsec = tv.tv_usec * 1000;
		pthread_mutex_lock( (pthread_mutex_t *)handle2 );
		while ( !flag && (res = pthread_cond_timedwait((pthread_cond_t *)handle, (pthread_mutex_t *)handle2, &ts)) == 0 )
			;
		if ( res == 0 && autoReset )
			flag = 0 ;
		pthread_mutex_unlock( (pthread_mutex_t *)handle2 );
	}
	return res == 0;
#endif
}

void Event::signal()
{
#ifdef _WIN32
	if ( handle != 0 )
		SetEvent( (HANDLE)handle );
#else
	pthread_mutex_lock( (pthread_mutex_t *)handle2 );
	flag = 1;
	pthread_cond_signal((pthread_cond_t *)handle);
	pthread_mutex_unlock( (pthread_mutex_t *)handle2 );
#endif
}

void Event::reset()
{
#ifdef _WIN32
	if ( handle != 0 )
		ResetEvent( (HANDLE)handle );
#else
	pthread_mutex_lock( (pthread_mutex_t *)handle2 );
	flag = 0;
	pthread_mutex_unlock( (pthread_mutex_t *)handle2 );
#endif
}

// Thread

#ifdef _WIN32
static DWORD threadProc( LPVOID param )
{
	static_cast<Thread *>(param)->work();
	return 0;
}
#else
static void *threadProc( void *param )
{
	static_cast<Thread *>(param)->work();
	return 0;
}
#endif

Thread::Thread( void *param ) : thrParam( param ), handle(0), shouldTerminate(0), killFlag(0)
{
}

// use this instead of dtor!
void Thread::destroy()
{
}

void Thread::wait()
{
	if ( !handle )
		return;			// not running
	destroy();
	shouldTerminate = 1;
#ifdef _WIN32
	WaitForSingleObject( (HANDLE)handle, INFINITE );
	CloseHandle( (HANDLE)handle );
#else
	void *vp = 0;
	if ( pthread_join( *(pthread_t *)handle, &vp ) !=  0 )
	{
#ifndef __ANDROID__
		pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, 0 );
		pthread_cancel( *(pthread_t *)handle );
#endif
	}
	delete (pthread_t *)handle;
#endif
	handle = 0;
	shouldTerminate = 0;
}

Thread::~Thread()
{
	assert( killFlag && "don't call delete directly!" );
}

void Thread::kill()
{
	killFlag = 1;
	wait();
	delete this;
}

// returns current thread id
void *Thread::current()
{
#ifdef _WIN32
	return (void *)(uintptr_t)GetCurrentThreadId();
#else
	return (void *)pthread_self();
#endif
}

// create and run thread
bool Thread::run()
{
	if ( handle )
		return 0;			// already running
#ifdef _WIN32
	handle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)threadProc, (LPVOID)this, 0/*CREATE_SUSPENDED*/, 0 );
#else
	handle = new pthread_t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create( (pthread_t *)handle, 0 /* attr */, threadProc, this );
	pthread_attr_destroy(&attr);
#endif
	return 1;
}

bool Thread::setPriority(int prior)
{
#ifdef _WIN32
	if ( !handle )
		return 0;
	if ( prior > 3 )
		prior = 3;
	else if ( prior < - 4 )
		prior = -4;
	int tp;
	switch( prior )
	{
		case +3:
			tp = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		case +2:
			tp = THREAD_PRIORITY_HIGHEST;
			break;
		case +1:
			tp = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case 0:
			tp = THREAD_PRIORITY_NORMAL;
			break;
		case -1:
			tp = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case -2:
			tp = THREAD_PRIORITY_LOWEST;
			break;
		case -3:
			tp = THREAD_PRIORITY_IDLE;
			break;
		default:
			tp = THREAD_PRIORITY_NORMAL;
	}
	return SetThreadPriority( (HANDLE)handle, tp ) == TRUE;
#else
	struct sched_param param;
	param.sched_priority = -prior;
	/*int ret =*/ pthread_setschedparam (*(pthread_t *)handle, SCHED_OTHER, &param);
	return 1;
#endif
}

// sleep in ms
void Thread::sleep( int ms )
{
#ifdef _WIN32
	Sleep( ms );
#else
	::usleep( ms * 1000 );
#endif
}

}
