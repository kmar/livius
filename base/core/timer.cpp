/*
livius - a TLCV-compatible live chess viewer

Copyright (c) 2014 Martin Sedlak (mar)

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
