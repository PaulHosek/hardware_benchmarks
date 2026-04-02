#include "arch_bench.hpp"

void BM_ILP_Dependent(benchmark::State& state) {
    uint64_t a = 1;
    for (auto _ : state) {
        for (int i = 0; i < 10000; ++i) {
            a = (a * 3) + 1;
        }
        benchmark::DoNotOptimize(a);
    }
    state.SetItemsProcessed(state.iterations() * 10000);
}

void BM_ILP_Independent(benchmark::State& state) {
    uint64_t a = 1, b = 1, c = 1, d = 1;   // 4 independent chains
    for (auto _ : state) {
        for (int i = 0; i < 10000; ++i) {
            a = a * 3;
            b = b + 1;
            c = c * 3;
            d = d + 1;
        }
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(c);
        benchmark::DoNotOptimize(d);
    }
    state.SetItemsProcessed(state.iterations() * 10000 * 2); // 2 ops per iteration
}

BENCHMARK(BM_ILP_Dependent)->Name("ILP_Dependent");
BENCHMARK(BM_ILP_Independent)->Name("ILP_Independent");