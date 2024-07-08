#if _WIN32

#include <intrin.h>
#include <windows.h>

u64 GetOSTimerFreq(void)
{
    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    return Freq.QuadPart;
}

u64 ReadOSTimer(void)
{
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    return Value.QuadPart;
}

#else

#include <x86intrin.h>
#include <sys/time.h>
#include "metrics.h"
#include <cstdio>

u64 GetOSTimerFreq(void)
{
    return 1000000;
}

u64 ReadOSTimer(void)
{
    struct timeval Value;
    gettimeofday(&Value, 0);
    u64 Result = GetOSTimerFreq() * (u64)Value.tv_sec + (u64)Value.tv_usec;
    return Result;
}

#endif

u64 ReadCPUTimer(void)
{
    return __rdtsc();
}
