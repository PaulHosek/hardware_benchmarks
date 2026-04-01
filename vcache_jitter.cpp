#include "arch_bench.hpp"

void GenerateVCacheCSV() {
    // 80mb: spans both the 32mb base l3 and the 64mb stacked v-cache
    const size_t size_mb = 80;
    const size_t num_nodes = (size_mb * 1024 * 1024) / sizeof(Node);
    const int iterations = 1'000'000;

    std::vector<Node> buffer(num_nodes);
    std::vector<size_t> indices(num_nodes);
    std::iota(indices.begin(), indices.end(), 0);

    // randomize
    std::mt19937 gen(42);
    std::shuffle(indices.begin(), indices.end(), gen);

    for (size_t i = 0; i < num_nodes - 1; ++i) {
        buffer[indices[i]].next = &buffer[indices[i+1]];
    }
    buffer[indices.back()].next = &buffer[indices[0]];

    Node* curr = &buffer[0];

    // fill tlb
    for(size_t i = 0; i < num_nodes; ++i) {
        curr = curr->next;
    }

    std::vector<uint64_t> samples;
    samples.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        _mm_lfence();
        uint64_t t1 = __rdtsc();
        _mm_lfence();

        curr = curr->next;

        _mm_lfence();
        uint64_t t2 = __rdtsc();
        _mm_lfence();

        samples.push_back(t2 - t1);
    }

    std::ofstream out("data/vcache_samples.csv");
    out << "cycles\n";
    for (auto s : samples) {
        out << s << "\n";
    }

}