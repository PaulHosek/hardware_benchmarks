#ifndef ARCH_BENCH_HPP
#define ARCH_BENCH_HPP

// architecture benchmarking imports

#include <benchmark/benchmark.h>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <atomic>
#include <thread>
#include <cstdint>
#include <fstream>
#include <x86intrin.h>
#include <iostream>

inline constexpr size_t CACHE_LINE_SIZE = std::hardware_constructive_interference_size;
inline constexpr size_t PAGE_SIZE = 4096;

struct Node {
    Node* next;
    char padding[CACHE_LINE_SIZE - sizeof(Node*)];
};

#endif
