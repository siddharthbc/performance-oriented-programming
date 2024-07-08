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

    // measure_cycles(SingleScalar, n, numbers.data(), "SingleScalar");
    

    return 0;
}
