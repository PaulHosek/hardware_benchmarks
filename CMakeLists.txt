cmake_minimum_required(VERSION 3.28)
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
        src/cache_latency.cpp
        src/tlb_walks.cpp
        src/branch_prediction.cpp
        src/false_sharing.cpp
        src/ilp_hazards.cpp
        src/vcache_jitter.cpp
)

target_compile_options(hardware_benchmarks PRIVATE -O3 -march=native -Wall)

target_link_libraries(hardware_benchmarks PRIVATE benchmark::benchmark)

if(WIN32)
    target_link_libraries(hardware_benchmarks PRIVATE shlwapi)
endif()