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
#include <windows.h>

inline constexpr size_t CACHE_LINE_SIZE = std::hardware_constructive_interference_size;
inline constexpr size_t PAGE_SIZE = 4096;

struct Node {
    Node* next;
    char padding[CACHE_LINE_SIZE - sizeof(Node*)];
};

inline bool EnableLargePagesPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }
    if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &tp.Privileges[0].Luid)) {
        CloseHandle(hToken);
        return false;
    }
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    bool result = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, 0);
    CloseHandle(hToken);
    return result && (GetLastError() == ERROR_SUCCESS);
}
#endif
