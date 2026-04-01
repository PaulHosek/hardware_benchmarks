cmake_minimum_required(VERSION 3.31)
project(hardware_benchmarks)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        google_benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.9.1
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(google_benchmark)

add_executable(hardware_benchmarks
        main.cpp
        cache_latency.cpp
        tlb_walks.cpp
        branch_prediction.cpp
        false_sharing.cpp
        ilp_hazards.cpp
        vcache_jitter.cpp
)

target_compile_options(hardware_benchmarks PRIVATE -O3 -march=native -Wall)

target_link_libraries(hardware_benchmarks PRIVATE benchmark::benchmark)

if(WIN32)
    target_link_libraries(hardware_benchmarks PRIVATE shlwapi)
endif()