#include "arch_bench.hpp"

void BM_TLB_Walk(benchmark::State& state) {
    const size_t stride = PAGE_SIZE;
    const size_t size   = 2ULL * 1024 * 1024 * 1024; // 2 GiB - should exceed both TLB

    std::vector<char> data(size, 0);

    for (auto _ : state) {
        for (size_t i = 0; i < size; i += stride) {
            for (size_t j = 0; j < CACHE_LINE_SIZE; j += sizeof(uint64_t)) {
                data[i + j]++;
                benchmark::DoNotOptimize(data[i + j]);
            }
            benchmark::DoNotOptimize(data[i]);
        }
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * (size / stride));
}

BENCHMARK(BM_TLB_Walk)->Name("TLB_Walk");



void GenerateTlbCSV() {
    std::ofstream out("data/tlb_sweep.csv");
    out << "size_bytes,cycles_per_access\n";

    const size_t page_size = PAGE_SIZE;

    // Sweep from ~16 KB up to ~4 GB -> should be enough to see L1 DTLB, L2 TLB, and full page-walk penalty
    for (size_t num_pages = 4; num_pages <= (4ULL * 1024 * 1024 * 1024) / page_size; num_pages += num_pages / 4) {
        size_t size_bytes = num_pages * page_size;

        std::vector<char> buffer(size_bytes);

        std::vector<size_t> indices(num_pages);
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), std::mt19937(42));

        for (size_t i = 0; i < num_pages - 1; ++i) {
            *reinterpret_cast<char**>(&buffer[indices[i] * page_size]) = &buffer[indices[i + 1] * page_size];
        }
        *reinterpret_cast<char**>(&buffer[indices.back() * page_size]) = &buffer[indices[0] * page_size];

        char* curr = &buffer[0];

        for (size_t i = 0; i < num_pages * 4; ++i) {
            curr = *reinterpret_cast<char**>(curr);
        }

        const int iterations = 10'000'000;

        _mm_lfence();
        uint64_t start = __rdtsc();
        _mm_lfence();

        for (int i = 0; i < iterations; ++i) {
            curr = *reinterpret_cast<char**>(curr);
            benchmark::DoNotOptimize(curr);
        }

        _mm_lfence();
        uint64_t end = __rdtsc();
        _mm_lfence();

        double cycles_per_access = static_cast<double>(end - start) / iterations;

        out << size_bytes << "," << cycles_per_access << "\n";
    }
}