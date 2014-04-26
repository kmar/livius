#include "timer.h"

// FIXME: gettimeofday is unreliable!!! change it! (really?)

#ifdef _WIN32
#define USE_TIMEGETTIME			// should be much more accurate, but... there's timeBeginPeriod which is potentially bad
#include <windows.h>
#undef min
#undef max
#include <time.h>

#if defined(USE_TIMEGETTIME) && !defined(__GNUC__)
#pragma comment(lib, "winmm.lib")
#endif

#else
#include <time.h>
#include <sys/time.h>
#endif

namespace core
{

// Timer

void Timer::init()
{
#if defined(_WIN32) && defined(USE_TIMEGETTIME)
	timeBeginPeriod(1);
#endif
}

void Timer::done()
{
#if defined(_WIN32) && defined(USE_TIMEGETTIME)
	timeEndPeriod(1);
#endif
}

i32 Timer::getMillisec()
{
#ifndef _WIN32
	struct timeval tp;
	struct timezone tzp;

	gettimeofday( &tp, &tzp );

	return (i32)((tp.tv_sec) * 1000 + tp.tv_usec / 1000);
#else

#ifdef USE_TIMEGETTIME
	return (i32)(timeGetTime() & 0xffffffffU);
#else
	return (i32)(GetTickCount() & 0xffffffffU);
#endif

#endif
}

}
