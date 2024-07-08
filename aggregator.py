import time
import numpy as np
import psutil

# Define the addition functions
def single_scalar(count, input_array):
    sum = 0
    for index in range(count):
        sum += input_array[index]
    return sum

def measure_cycles(func, count, input_array, cpu_frequency, name):
    start_time = time.perf_counter()
    func(count, input_array)
    end_time = time.perf_counter()

    elapsed_time = end_time - start_time
    total_cycles = elapsed_time * cpu_frequency
    adds_per_cycle = count / total_cycles

    print(f"{name} - Total clock cycles: {total_cycles}")
    print(f"{name} - Adds per cycle: {adds_per_cycle}")
    print(f"{name} - Total Time: {elapsed_time}")
if __name__ == "__main__":
    n = 1000000
    numbers = np.ones(n, dtype=np.uint32)
    # Get the current CPU frequency in Hz
    cpu_frequency = psutil.cpu_freq().current * 1e6  # Convert MHz to Hz

    measure_cycles(single_scalar, n, numbers, cpu_frequency, "SingleScalar")
