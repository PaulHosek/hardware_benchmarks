
#include "arch_bench.hpp"


void BM_CacheHierarchy(benchmark::State& state) {
    size_t num_nodes = state.range(0) / sizeof(Node);
    std::vector<Node> buffer(num_nodes);
    std::vector<size_t> indices(num_nodes);

    std::iota(indices.begin(), indices.end(), 0);
    std::mt19937 gen(42);
    std::shuffle(indices.begin(), indices.end(), gen);

    for (size_t i = 0; i < num_nodes - 1; ++i) {
        buffer[indices[i]].next = &buffer[indices[i + 1]];
    }
    buffer[indices.back()].next = &buffer[indices[0]];

    Node* current = &buffer[indices[0]];
    for (auto _ : state) {
        current = current->next;
        benchmark::DoNotOptimize(current);
    }
}
BENCHMARK(BM_CacheHierarchy)->RangeMultiplier(2)->Range(4096, 256 << 20);



void GenerateCacheCSV() {
    std::ofstream out("data/cache_sweep.csv");
    out << "size_bytes,cycles_per_access\n";

    // Sweep from 4KB up to 256MB with smaller steps for higher resolution
    for (size_t size = 4096; size <= (256ULL * 1024 * 1024); size += size / 4) {
        size_t num_nodes = size / sizeof(Node);
        std::vector<Node> buffer(num_nodes);
        std::vector<size_t> indices(num_nodes);

        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), std::mt19937(42));

        for (size_t i = 0; i < num_nodes - 1; ++i) {
            buffer[indices[i]].next = &buffer[indices[i+1]];
        }
        buffer[indices.back()].next = &buffer[indices[0]];

        Node* curr = &buffer[0];

        // Warmup
        for(size_t i = 0; i < num_nodes; ++i) curr = curr->next;

        const int iterations = 10'000'000;

        _mm_lfence();
        uint64_t start = __rdtsc();
        _mm_lfence();

        for (int i = 0; i < iterations; ++i) {
            curr = curr->next;
            benchmark::DoNotOptimize(curr);
        }

        _mm_lfence();
        uint64_t end = __rdtsc();
        _mm_lfence();

        double cycles_per_access = static_cast<double>(end - start) / iterations;
        out << size << "," << cycles_per_access << "\n";
    }
}