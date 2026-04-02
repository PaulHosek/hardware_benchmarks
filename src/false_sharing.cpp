#include "arch_bench.hpp"

struct Packed {
    std::atomic<int> a{0};
    std::atomic<int> b{0};
};

struct Padded {
    std::atomic<int> a{0};
    alignas(CACHE_LINE_SIZE) std::atomic<int> b{0};
};

template <typename T>
void BM_FalseSharing(benchmark::State& state) {
    T data;

    std::atomic<bool> keep_running{true};
    std::atomic<int> work_round{0};
    std::atomic<int> completed_rounds{0};

    auto worker = [&](std::atomic<int>& target) {
        int last_round = 0;
        while (keep_running.load(std::memory_order_relaxed)) {
            // Wait for a new round to start
            while (work_round.load(std::memory_order_relaxed) == last_round) {
                std::this_thread::yield();
            }
            last_round = work_round.load(std::memory_order_relaxed);

            for (int i = 0; i < 1'000'000; ++i) {
                target.fetch_add(1, std::memory_order_relaxed);
            }

            completed_rounds.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread t1(worker, std::ref(data.a));
    std::thread t2(worker, std::ref(data.b));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    for (auto _ : state) {
        work_round.fetch_add(1, std::memory_order_release);

        while (completed_rounds.load(std::memory_order_acquire) < 2) {
            std::this_thread::yield();
        }
        completed_rounds.store(0, std::memory_order_relaxed);
    }

    keep_running.store(false, std::memory_order_relaxed);
    work_round.fetch_add(1, std::memory_order_release);

    t1.join();
    t2.join();

    state.SetItemsProcessed(state.iterations() * 2'000'000LL);
}

BENCHMARK(BM_FalseSharing<Packed>)->Name("FalseSharing_Conflict");
BENCHMARK(BM_FalseSharing<Padded>)->Name("FalseSharing_Aligned");