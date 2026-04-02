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
    out << "size_bytes,cycles_std,cycles_large\n";

    bool largePagesEnabled = EnableLargePagesPrivilege();
    SIZE_T largePageMin = largePagesEnabled ? GetLargePageMinimum() : 0;

    if (!largePagesEnabled || largePageMin == 0) {
        std::cerr << "WARNING: Large Pages not enabled or supported. Large page column will be 0.\n";
        std::cerr << "Did you set the 'Lock pages in memory' Group Policy and restart?\n";
    }

    for (size_t size = 4096; size <= (256ULL * 1024 * 1024); size += size / 4) {
        size_t num_nodes = size / sizeof(Node);

        size_t iterations = std::max<size_t>(10'000'000, num_nodes * 4);

        // ---------------------------------------------------------
        // 1. STANDARD PAGES (std::vector)
        std::vector<Node> buffer_std(num_nodes);
        std::vector<size_t> indices(num_nodes);
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), std::mt19937(42));

        for (size_t i = 0; i < num_nodes - 1; ++i) {
            buffer_std[indices[i]].next = &buffer_std[indices[i+1]];
        }
        buffer_std[indices.back()].next = &buffer_std[indices[0]];

        Node* curr_std = &buffer_std[0];
        for(size_t i = 0; i < num_nodes; ++i) curr_std = curr_std->next;

        _mm_lfence();
        uint64_t start_std = __rdtsc();
        _mm_lfence();

        for (size_t i = 0; i < iterations; ++i) {
            curr_std = curr_std->next;
            benchmark::DoNotOptimize(curr_std);
        }

        _mm_lfence();
        uint64_t end_std = __rdtsc();
        _mm_lfence();

        double cycles_std = static_cast<double>(end_std - start_std) / iterations;

        // 2. LARGE PAGES (VirtualAlloc)
        double cycles_large = 0.0;

        if (largePagesEnabled) {
            SIZE_T bytes_needed = num_nodes * sizeof(Node);
            SIZE_T alloc_size = (bytes_needed + largePageMin - 1) & ~(largePageMin - 1);

            Node* buffer_large = static_cast<Node*>(VirtualAlloc(
                NULL, alloc_size, MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES, PAGE_READWRITE));

            if (buffer_large) {
                for (size_t i = 0; i < num_nodes - 1; ++i) {
                    buffer_large[indices[i]].next = &buffer_large[indices[i+1]];
                }
                buffer_large[indices.back()].next = &buffer_large[indices[0]];

                Node* curr_large = &buffer_large[0];
                for(size_t i = 0; i < num_nodes; ++i) curr_large = curr_large->next;

                _mm_lfence();
                uint64_t start_lp = __rdtsc();
                _mm_lfence();

                for (size_t i = 0; i < iterations; ++i) {
                    curr_large = curr_large->next;
                    benchmark::DoNotOptimize(curr_large);
                }

                _mm_lfence();
                uint64_t end_lp = __rdtsc();
                _mm_lfence();

                cycles_large = static_cast<double>(end_lp - start_lp) / iterations;
                VirtualFree(buffer_large, 0, MEM_RELEASE);
            }
        }

        out << size << "," << cycles_std << "," << cycles_large << "\n";
        std::cout << "Swept size: " << size / 1024 << " KB\r";
    }
    std::cout << "\nDone." << std::endl;
}