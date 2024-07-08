#ifndef METRICS_H
#define METRICS_H

#include <cstdint>

// Define u64 as uint64_t and f64 as double
using u64 = uint64_t;

u64 GetOSTimerFreq(void);
u64 ReadOSTimer(void);
u64 ReadCPUTimer(void);

#endif // METRICS_H