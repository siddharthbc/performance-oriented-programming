#include "metrics.h"
#include <cstdio>

using u64 = uint64_t;
using f64 = double;

#ifndef PROFILER
#define PROFILER 0
#endif

#ifndef READ_BLOCK_TIMER
#define READ_BLOCK_TIMER ReadCPUTimer
#endif

#if PROFILER

struct profile_anchor
{
    u64 TSCElapsedExclusive;
    u64 TSCElapsedInclusive;
    u64 HitCount;
    const char *Label;
};
static profile_anchor GlobalProfilerAnchors[4096];
static u32 GlobalProfilerParent;

struct profile_block
{
    profile_block(const char *Label_, u32 AnchorIndex_)
    {
        ParentIndex = GlobalProfilerParent;
        AnchorIndex = AnchorIndex_;
        Label = Label_;
        profile_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
        OldTSCElapsedInclusive = Anchor->TSCElapsedInclusive;
        GlobalProfilerParent = AnchorIndex;
        StartTSC = READ_BLOCK_TIMER();
    }
    ~profile_block()
    {
        u64 Elapsed = READ_BLOCK_TIMER() - StartTSC;
        GlobalProfilerParent = ParentIndex;
        profile_anchor *Parent = GlobalProfilerAnchors + ParentIndex;
        profile_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
        Parent->TSCElapsedExclusive -= Elapsed;
        Anchor->TSCElapsedExclusive += Elapsed;
        Anchor->TSCElapsedInclusive = OldTSCElapsedInclusive + Elapsed;
        ++Anchor->HitCount;
        Anchor->Label = Label;
    }
    const char *Label;
    u64 OldTSCElapsedInclusive;
    u64 StartTSC;
    u32 ParentIndex;
    u32 AnchorIndex;
};

#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)
#define TimeBlock(Name) profile_block NameConcat(Block, __LINE__)(Name, __COUNTER__ + 1);
#define ProfilerEndOfCompilationUnit static_assert(__COUNTER__ < ArrayCount(GlobalProfilerAnchors), "Number of profile points exceeds size of profiler::Anchors array")

static void PrintTimeElapsed(u64 TotalTSCElapsed, profile_anchor *Anchor)
{
    f64 Percent = 100.0 * ((f64)Anchor->TSCElapsedExclusive / (f64)TotalTSCElapsed);
    printf("  %s[%llu]: %llu (%.2f%%", Anchor->Label, Anchor->HitCount, Anchor->TSCElapsedExclusive, Percent);
    if (Anchor->TSCElapsedInclusive != Anchor->TSCElapsedExclusive)
    {
        f64 PercentWithChildren = 100.0 * ((f64)Anchor->TSCElapsedInclusive / (f64)TotalTSCElapsed);
        printf(", %.2f%% w/children", PercentWithChildren);
    }
    printf(")\n");
}

static void PrintAnchorData(u64 TotalCPUElapsed)
{
    for (u32 AnchorIndex = 0; AnchorIndex < ArrayCount(GlobalProfilerAnchors); ++AnchorIndex)
    {
        profile_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
        if (Anchor->TSCElapsedInclusive)
        {
            PrintTimeElapsed(TotalCPUElapsed, Anchor);
        }
    }
}

#else

#define TimeBlock(...)
#define PrintAnchorData(...)
#define ProfilerEndOfCompilationUnit

#endif

struct profiler
{
    u64 StartTSC;
    u64 EndTSC;
};
static profiler GlobalProfiler;

#define TimeFunction TimeBlock(__func__)

static u64 EstimateBlockTimerFreq(void)
{
    u64 MillisecondsToWait = 100;
    u64 OSFreq = GetOSTimerFreq();

    u64 BlockStart = READ_BLOCK_TIMER();
    u64 OSStart = ReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
    while (OSElapsed < OSWaitTime)
    {
        OSEnd = ReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }

    u64 BlockEnd = READ_BLOCK_TIMER();
    u64 BlockElapsed = BlockEnd - BlockStart;

    u64 BlockFreq = 0;
    if (OSElapsed)
    {
        BlockFreq = OSFreq * BlockElapsed / OSElapsed;
    }

    return BlockFreq;
}

static void BeginProfile(void)
{
    GlobalProfiler.StartTSC = READ_BLOCK_TIMER();
}

static u64 EndAndPrintProfile()
{
    GlobalProfiler.EndTSC = READ_BLOCK_TIMER();
    u64 TimerFreq = EstimateBlockTimerFreq();

    u64 TotalTSCElapsed = GlobalProfiler.EndTSC - GlobalProfiler.StartTSC;
    // printf("\nTotal clock cycles: %llu\n", TotalTSCElapsed);
    if (TimerFreq)
    {
        printf("\nTotal time: %0.4fms (timer freq %lu)\n", 1000.0 * (f64)TotalTSCElapsed / (f64)TimerFreq, TimerFreq);
    }

    PrintAnchorData(TotalTSCElapsed);
    return TotalTSCElapsed;
}
