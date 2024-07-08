# performance-oriented-programming

# Aggregator Profiler Project

This project measures the performance of different aggregation functions using a custom profiler. It includes a main application (`aggregator.cpp`) and profiling utilities (`metrics.cpp`, `metrics.h`, `profile.cpp`).

## Prerequisites

To compile and run this project, ensure you have the following installed:

1. **C++ Compiler**: GCC or Clang with support for C++11 or higher.
2. **Git**: To clone the repository.
3. **Operating System**: Linux (Note: Even though we have support for windows code it requires specific headers and functions). 
4. **GDB**: To disassemble and inspect instructions.

## Project Structure

- `aggregator.cpp`: Main application file.
- `metrics.cpp`: Contains functions to measure time and CPU cycles.
- `metrics.h`: Header file for `metrics.cpp`.
- `profile.cpp`: Implementation of the custom profiler.

## Cloning the Repository

```sh
git clone <repository-url>
cd <repository-directory>
```
## Compiling the Project
# Linux
# Ensure you have GCC or Clang installed. Run the following command to compile the project:

```sh
g++ -o aggregator aggregator.cpp metrics.cpp profile.cpp -lpthread
```

Ensure you have a suitable compiler like MSVC. Open the Developer Command Prompt and run:

```sh
cl /EHsc aggregator.cpp metrics.cpp profile.cpp
```

Running the Application
```sh
./aggregator <number of elements>
```
Example
```sh
./aggregator 1000
```