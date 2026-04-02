#include <benchmark/benchmark.h>
#include <iostream>
#include <filesystem> // Added for directory management

void GenerateCacheCSV();
void GenerateBranchCSV();
void GenerateVCacheCSV();
void GenerateTlbCSV();
void GenerateILPCSV();





int main(int argc, char** argv) {
    // google benchmarks
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    // python
    std::filesystem::create_directories("data");
    GenerateCacheCSV();
    GenerateBranchCSV();
    GenerateVCacheCSV();
    GenerateTlbCSV();
    GenerateILPCSV();
    return 0;
}