#include "arch_bench.hpp"

void GenerateVCacheCSV() {
    const size_t size_mb = 80;
    const size_t num_nodes = (size_mb * 1024ULL * 1024ULL) / sizeof(Node);

    // We will measure batches of 16 accesses to defeat rdtsc overhead
    // while preserving outlier spikes.
    const int batch_size = 16;
    const int total_samples = 1'000'000;

    // 1. ALLOCATE WITH LARGE PAGES
    EnableLargePagesPrivilege();
    SIZE_T largePageMin = GetLargePageMinimum();
    SIZE_T bytes_needed = num_nodes * sizeof(Node);
    SIZE_T alloc_size = (bytes_needed + largePageMin - 1) & ~(largePageMin - 1);

    Node* buffer = static_cast<Node*>(VirtualAlloc(
        NULL, alloc_size, MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES, PAGE_READWRITE));

    if (!buffer) {
        std::cerr << "Large Pages failed! Fallback to standard vector is not recommended for this test." << std::endl;
        return;
    }

    // 2. SETUP & SHUFFLE
    std::vector<size_t> indices(num_nodes);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), std::mt19937(42));

    for (size_t i = 0; i < num_nodes - 1; ++i) {
        buffer[indices[i]].next = &buffer[indices[i+1]];
    }
    buffer[indices.back()].next = &buffer[indices[0]];

    Node* curr = buffer;

    // 3. WARMUP (fill the cache and TLB)
    for(size_t i = 0; i < num_nodes; ++i) {
        curr = curr->next;
    }

    std::vector<double> samples;
    samples.reserve(total_samples);

    // 4. THE MICRO-BATCH MEASUREMENT
    for (int i = 0; i < total_samples; ++i) {
        _mm_lfence();
        uint64_t t1 = __rdtsc();
        _mm_lfence();

        // Strict data dependency handles the serialization naturally!
        curr = curr->next->next->next->next
                   ->next->next->next->next
                   ->next->next->next->next
                   ->next->next->next->next; // Unrolled 16 times

        _mm_lfence();
        uint64_t t2 = __rdtsc();
        _mm_lfence();

        // Divide total cycles by 16 to get the per-access latency for this batch
        samples.push_back(static_cast<double>(t2 - t1) / batch_size);
    }

    // 5. OUTPUT
    std::ofstream out("data/vcache_samples.csv");
    out << "cycles\n";
    for (auto s : samples) {
        out << s << "\n";
    }

    VirtualFree(buffer, 0, MEM_RELEASE);
}