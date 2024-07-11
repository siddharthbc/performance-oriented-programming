#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>
#include <tmmintrin.h>
#include <thread>
#include <stdexcept>

// Profiler implementation
#include "profiler.cpp"

using u32 = unsigned int;

typedef unsigned int u32;

u32 SingleScalar(u32 Count, u32 *Input) {
    // TimeBlock("SingleScalar");
    u32 Sum = 0;
    for (u32 Index = 0; Index < Count; ++Index) {
        Sum += Input[Index];
    }
    return Sum;
}

u32 Unroll2Scalar(u32 Count, u32 *Input) {
    // TimeBlock("Unroll2Scalar");
    u32 Sum = 0;
    for (u32 Index = 0; Index < Count; Index += 2) {
        Sum += Input[Index];
        Sum += Input[Index + 1];
    }
    return Sum;
}

u32 Unroll4Scalar(u32 Count, u32 *Input)
{
    u32 Sum = 0;
    for(u32 Index = 0; Index < Count; Index += 4)
    {
        Sum += Input[Index];
        Sum += Input[Index + 1];
        Sum += Input[Index + 2];
        Sum += Input[Index + 3];
    }
    return Sum;
}

u32 DualScalar(u32 Count, u32 *Input) {
    TimeBlock("DualScalar");
    u32 SumA = 0;
    u32 SumB = 0;
    for (u32 Index = 0; Index < Count; Index += 2) {
        SumA += Input[Index];
        SumB += Input[Index + 1];
    }
    return SumA + SumB;
}

u32 QuadScalar(u32 Count, u32 *Input) {
    u32 SumA = 0;
    u32 SumB = 0;
    u32 SumC = 0;
    u32 SumD = 0;
    for (u32 Index = 0; Index < Count; Index += 4) {
        SumA += Input[Index];
        SumB += Input[Index + 1];
        SumC += Input[Index + 2];
        SumD += Input[Index + 3];
    }
    return SumA + SumB + SumC + SumD;
}

u32 QuadScalarPtr(u32 Count, u32 *Input) {
    TimeBlock("QuadScalarPtr");
    u32 SumA = 0;
    u32 SumB = 0;
    u32 SumC = 0;
    u32 SumD = 0;
    Count /= 4;
    while (Count--) {
        SumA += Input[0];
        SumB += Input[1];
        SumC += Input[2];
        SumD += Input[3];
        Input += 4;
    }
    return SumA + SumB + SumC + SumD;
}

u32 TreeScalarPtr(u32 Count, u32 *Input) {
    TimeBlock("TreeScalarPtr");
    u32 Sum = 0;
    Count /= 4;
    while (Count--) {
        u32 TempA = Input[0] + Input[1];
        u32 TempB = Input[2] + Input[3];
        u32 Temp = TempA + TempB;
        Sum += Temp;
        Input += 4;
    }
    return Sum;
}

u32 __attribute__((target("ssse3"))) SingleSSE(u32 Count, u32 *Input) {
    // TimeBlock("SingleSSE");
    __m128i Sum = _mm_setzero_si128();
    for (u32 Index = 0; Index < Count; Index += 4) {
        Sum = _mm_add_epi32(Sum, _mm_loadu_si128((__m128i *)&Input[Index]));
    }
    Sum = _mm_hadd_epi32(Sum, Sum);
    Sum = _mm_hadd_epi32(Sum, Sum);
    return _mm_cvtsi128_si32(Sum);
}

u32 __attribute__((target("avx2"))) SingleAVX(u32 Count, u32 *Input) {
    // TimeBlock("SingleAVX");
    __m256i Sum = _mm256_setzero_si256();
    for (u32 Index = 0; Index < Count; Index += 8) {
        Sum = _mm256_add_epi32(Sum, _mm256_loadu_si256((__m256i *)&Input[Index]));
    }
    Sum = _mm256_hadd_epi32(Sum, Sum);
    Sum = _mm256_hadd_epi32(Sum, Sum);
    __m256i SumS = _mm256_permute2x128_si256(Sum, Sum, 1 | (1 << 4));
    Sum = _mm256_add_epi32(Sum, SumS);
    return _mm256_cvtsi256_si32(Sum);
}



u32 __attribute__((target("avx2"))) DualAVX(u32 Count, u32 *Input) {
    TimeBlock("DualAVX");
    __m256i SumA = _mm256_setzero_si256();
    __m256i SumB = _mm256_setzero_si256();
    for (u32 Index = 0; Index < Count; Index += 16) {
        SumA = _mm256_add_epi32(SumA, _mm256_loadu_si256((__m256i *)&Input[Index]));
        SumB = _mm256_add_epi32(SumB, _mm256_loadu_si256((__m256i *)&Input[Index + 8]));
    }
    __m256i Sum = _mm256_add_epi32(SumA, SumB);
    Sum = _mm256_hadd_epi32(Sum, Sum);
    Sum = _mm256_hadd_epi32(Sum, Sum);
    __m256i SumS = _mm256_permute2x128_si256(Sum, Sum, 1 | (1 << 4));
    Sum = _mm256_add_epi32(Sum, SumS);
    return _mm256_cvtsi256_si32(Sum);
}

u32 __attribute__((target("avx2"))) QuadAVX(u32 Count, u32 *Input) {
    TimeBlock("QuadAVX");
    __m256i SumA = _mm256_setzero_si256();
    __m256i SumB = _mm256_setzero_si256();
    __m256i SumC = _mm256_setzero_si256();
    __m256i SumD = _mm256_setzero_si256();
    for (u32 Index = 0; Index < Count; Index += 32) {
        SumA = _mm256_add_epi32(SumA, _mm256_loadu_si256((__m256i *)&Input[Index]));
        SumB = _mm256_add_epi32(SumB, _mm256_loadu_si256((__m256i *)&Input[Index + 8]));
        SumC = _mm256_add_epi32(SumC, _mm256_loadu_si256((__m256i *)&Input[Index + 16]));
        SumD = _mm256_add_epi32(SumD, _mm256_loadu_si256((__m256i *)&Input[Index + 24]));
    }
    __m256i SumAB = _mm256_add_epi32(SumA, SumB);
    __m256i SumCD = _mm256_add_epi32(SumC, SumD);
    __m256i Sum = _mm256_add_epi32(SumAB, SumCD);
    Sum = _mm256_hadd_epi32(Sum, Sum);
    Sum = _mm256_hadd_epi32(Sum, Sum);
    __m256i SumS = _mm256_permute2x128_si256(Sum, Sum, 1 | (1 << 4));
    Sum = _mm256_add_epi32(Sum, SumS);
    return _mm256_cvtsi256_si32(Sum);
}

u32 __attribute__((target("avx2"))) QuadAVXPtr(u32 Count, u32 *Input) {
    TimeBlock("QuadAVXPtr");
    __m256i SumA = _mm256_setzero_si256();
    __m256i SumB = _mm256_setzero_si256();
    __m256i SumC = _mm256_setzero_si256();
    __m256i SumD = _mm256_setzero_si256();
    Count /= 32;
    while (Count--) {
        SumA = _mm256_add_epi32(SumA, _mm256_loadu_si256((__m256i *)&Input[0]));
        SumB = _mm256_add_epi32(SumB, _mm256_loadu_si256((__m256i *)&Input[8]));
        SumC = _mm256_add_epi32(SumC, _mm256_loadu_si256((__m256i *)&Input[16]));
        SumD = _mm256_add_epi32(SumD, _mm256_loadu_si256((__m256i *)&Input[24]));
        Input += 32;
    }
    __m256i SumAB = _mm256_add_epi32(SumA, SumB);
    __m256i SumCD = _mm256_add_epi32(SumC, SumD);
    __m256i Sum = _mm256_add_epi32(SumAB, SumCD);
    Sum = _mm256_hadd_epi32(Sum, Sum);
    Sum = _mm256_hadd_epi32(Sum, Sum);
    __m256i SumS = _mm256_permute2x128_si256(Sum, Sum, 1 | (1 << 4));
    Sum = _mm256_add_epi32(Sum, SumS);
    return _mm256_cvtsi256_si32(Sum);
}

unsigned long long get_clock_cycles() {
    unsigned int lo, hi;
    asm volatile (
        "rdtsc" : "=a" (lo), "=d" (hi)
    );
    return ((unsigned long long)hi << 32) | lo;
}

u32 ParallelQuadAVXPtr(u32 Count, u32 *Input) {
    // TimeBlock("ParallelQuadAVXPtr");
    unsigned int num_threads = 8;
    std::vector<std::thread> threads;
    std::vector<u32> results(num_threads, 0);

    u32 chunk_size = Count / num_threads;

    auto thread_func = [&results](u32* data, u32 size, u32 thread_index) {
        results[thread_index] = QuadAVXPtr(size, data);
    };

    for (unsigned int i = 0; i < num_threads; ++i) {
        u32* chunk_start = Input + i * chunk_size;
        u32 actual_chunk_size = (i == num_threads - 1) ? (Count - i * chunk_size) : chunk_size;
        threads.emplace_back(thread_func, chunk_start, actual_chunk_size, i);
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    u32 total_sum = 0;
    for (const auto& res : results) {
        total_sum += res;
    }

    return total_sum;
}


void measure_cycles(u32 (*func)(u32, u32*), u32 Count, u32* Input, const char* name) {
    std::cout << "---------- " << name << " Start ------------" << std::endl;
    BeginProfile();
    func(Count, Input);
    u64 TotalTSCElapsed = EndAndPrintProfile();
    double adds_per_cycle = static_cast<double>(Count) / TotalTSCElapsed;
    printf("\nAdds per cycle: %f\n", adds_per_cycle);
    std::cout << "---------- " << name << " End ------------" << std::endl;
}



int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number of elements>" << std::endl;
        return 1;
    }

    u32 n = std::atoi(argv[1]);
    std::vector<u32> numbers(n, 1);

    measure_cycles(SingleScalar, n, numbers.data(), "SingleScalar");
    measure_cycles(Unroll2Scalar, n, numbers.data(), "Unroll2Scalar");
    measure_cycles(Unroll4Scalar, n, numbers.data(), "Unroll4Scalar");
    measure_cycles(DualScalar, n, numbers.data(), "DualScalar");
    measure_cycles(QuadScalar, n, numbers.data(), "QuadScalar");
    measure_cycles(QuadScalarPtr, n, numbers.data(), "QuadScalarPtr");
    measure_cycles(TreeScalarPtr, n, numbers.data(), "TreeScalarPtr");
    measure_cycles(SingleSSE, n, numbers.data(), "SingleSSE");
    measure_cycles(SingleAVX, n, numbers.data(), "SingleAVX");
    measure_cycles(DualAVX, n, numbers.data(), "DualAVX");
    measure_cycles(QuadAVX, n, numbers.data(), "QuadAVX");
    measure_cycles(QuadAVXPtr, n, numbers.data(), "QuadAVXPtr");
    measure_cycles(ParallelQuadAVXPtr, n, numbers.data(), "ParallelQuadAVXPtr");
    

    return 0;
}
