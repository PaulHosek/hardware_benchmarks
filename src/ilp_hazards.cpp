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
    state.SetItemsProcessed(state.iterations() * 10000 * 2);
}

BENCHMARK(BM_ILP_Dependent)->Name("ILP_Dependent");
BENCHMARK(BM_ILP_Independent)->Name("ILP_Independent");

// A fast, non-collapsible operation (Xorshift variant)
// 2 shifts and 2 xors per step. Cannot be algebra-optimized by the compiler!
#define OP(x) x ^= (x << 13); x ^= (x >> 7)

void GenerateILPCSV() {
    std::ofstream out("data/ilp_sweep.csv");
    out << "chains,cycles_per_iteration\n";

    volatile int dyn_iterations = 20'000'000;
    int iterations = dyn_iterations;

    for (int chains = 1; chains <= 12; ++chains) {
        uint64_t c1=1, c2=2, c3=3, c4=4, c5=5, c6=6, c7=7, c8=8, c9=9, c10=10, c11=11, c12=12;

        _mm_lfence();
        uint64_t start = __rdtsc();
        _mm_lfence();

        switch(chains) {
            case 1:  for(int i=0; i<iterations; ++i) { OP(c1); } break;
            case 2:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); } break;
            case 3:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); } break;
            case 4:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); } break;
            case 5:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); } break;
            case 6:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); } break;
            case 7:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); OP(c7); } break;
            case 8:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); OP(c7); OP(c8); } break;
            case 9:  for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); OP(c7); OP(c8); OP(c9); } break;
            case 10: for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); OP(c7); OP(c8); OP(c9); OP(c10); } break;
            case 11: for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); OP(c7); OP(c8); OP(c9); OP(c10); OP(c11); } break;
            case 12: for(int i=0; i<iterations; ++i) { OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); OP(c7); OP(c8); OP(c9); OP(c10); OP(c11); OP(c12); } break;
        }

        _mm_lfence();
        uint64_t end = __rdtsc();
        _mm_lfence();

        benchmark::DoNotOptimize(c1); benchmark::DoNotOptimize(c2); benchmark::DoNotOptimize(c3);
        benchmark::DoNotOptimize(c4); benchmark::DoNotOptimize(c5); benchmark::DoNotOptimize(c6);
        benchmark::DoNotOptimize(c7); benchmark::DoNotOptimize(c8); benchmark::DoNotOptimize(c9);
        benchmark::DoNotOptimize(c10); benchmark::DoNotOptimize(c11); benchmark::DoNotOptimize(c12);

        double cycles_per_iter = static_cast<double>(end - start) / iterations;
        out << chains << "," << cycles_per_iter << "\n";
        std::cout << "Chains: " << chains << " -> " << cycles_per_iter << " cycles/iteration\n";
    }
}