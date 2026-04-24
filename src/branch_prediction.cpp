#include "arch_bench.hpp"

void BM_BranchPrediction(benchmark::State& state) {
    bool sorted = state.range(0);
    std::vector<int> data(1'000'000);
    std::generate(data.begin(), data.end(), []() { return rand() % 256; });
    if (sorted) std::sort(data.begin(), data.end());

    long long sum = 0;
    for (auto _ : state) {
        sum = 0;
        for (int val : data) {
            if (val > 128) sum += val;
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_BranchPrediction)->Arg(0)->Arg(1);



void GenerateBranchCSV() {
    std::ofstream out("data/branch_data.csv");
    out << "condition,cycles\n";

    const size_t size = 100'000;
    std::vector<int> sorted_data(size);
    std::vector<int> unsorted_data(size);

    std::mt19937 gen(42);
    for (size_t i = 0; i < size; ++i) {
        int val = gen() % 256;
        sorted_data[i] = val;
        unsorted_data[i] = val;
    }
    std::sort(sorted_data.begin(), sorted_data.end());

    auto measure = [&](const std::vector<int>& data, const std::string& name) {
        for (int run = 0; run < 100; ++run) {
            volatile long long sum = 0;

            _mm_lfence();
            uint64_t start = __rdtsc();
            _mm_lfence();

            for (int val : data) {
                if (val > 128) sum += val;
                benchmark::DoNotOptimize(sum);
            }

            _mm_lfence();
            uint64_t end = __rdtsc();
            _mm_lfence();

            out << name << "," << (end - start) << "\n";
            benchmark::DoNotOptimize(sum);
        }
    };

    measure(sorted_data, "Sorted (Predicted)");
    measure(unsorted_data, "Unsorted (Mispredicted)");

}
