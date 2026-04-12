// Timer.cpp: implementation of the CTimer class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Timer.h"

#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// Android implementation using clock_gettime(CLOCK_MONOTONIC)
// ─────────────────────────────────────────────────────────────────────────────
#include <time.h>

static inline double AndroidGetMs()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1.0e6;
}

CTimer::CTimer()
{
    m_bUsePerformanceCounter = TRUE;
    m_mmAbsTimerStart = m_mmTimerStart = (DWORD)AndroidGetMs();
    m_pcAbsTimerStart = m_pcTimerStart = (int64_t)(AndroidGetMs() * 1000.0);
    m_frequency  = 1000000LL; // microseconds
    m_resolution = 1.0f / 1000.0f; // 1 ms
}

CTimer::~CTimer() {}

double CTimer::GetTimeElapsed()
{
    return AndroidGetMs() - (double)m_mmTimerStart;
}

double CTimer::GetAbsTime()
{
    return AndroidGetMs();
}

void CTimer::ResetTimer()
{
    m_mmTimerStart = (DWORD)AndroidGetMs();
}

#else // Windows

CTimer::CTimer()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency) == FALSE)
	{
		m_bUsePerformanceCounter = FALSE;

		TIMECAPS Caps;
		::timeGetDevCaps(&Caps, sizeof(Caps));

		if (::timeBeginPeriod(Caps.wPeriodMin) == TIMERR_NOCANDO)
		{
#ifdef __TIMER_DEBUG
			__TraceF(TEXT("timeBeginPeriod(...) Error\n"));
#endif //__TIMER_DEBUG
		}

		m_mmAbsTimerStart = m_mmTimerStart = ::timeGetTime();
	}
	else
	{
		m_bUsePerformanceCounter = TRUE;

		::QueryPerformanceCounter((LARGE_INTEGER*)&m_pcTimerStart);

		m_pcAbsTimerStart = m_pcTimerStart;
		m_resolution = (float)(1.0 / (double)m_frequency) * 1000.0f;
	}
}

CTimer::~CTimer()
{
	if (!m_bUsePerformanceCounter)
	{
		TIMECAPS Caps;
		::timeGetDevCaps(&Caps, sizeof(Caps));
		::timeEndPeriod(Caps.wPeriodMin);
	}
}

double CTimer::GetTimeElapsed()
{
	__int64 timeElapsed;

	if (m_bUsePerformanceCounter)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&timeElapsed);
		timeElapsed -= m_pcTimerStart;
		return (double)timeElapsed * (double)m_resolution;
	}
	else
	{
		timeElapsed = ::timeGetTime() - m_mmTimerStart;
		return (double)timeElapsed;
	}
}

double CTimer::GetAbsTime()
{
	__int64 absTime;

	if (m_bUsePerformanceCounter)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&absTime);
		return (double)absTime * (double)m_resolution;
	}
	else
	{
		absTime = ::timeGetTime();
		return (double)absTime;
	}
}

void CTimer::ResetTimer()
{
	if (m_bUsePerformanceCounter)
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_pcTimerStart);
	else
		m_mmTimerStart = ::timeGetTime();
}

#endif // __ANDROID__
