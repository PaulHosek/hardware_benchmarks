# CPU Microarchitecture Benchmark Suite

I keep on reading things like "L1 data cache access times are 10-20ns." under which conditions, warm/cold, how did they measure it,...
and similar claims but what about my machine?
I used C++23, Google benchmark and RDTSC cycle counting for the measurements. My CPU is a AMD Ryzen 7 9800X3D (Zen 5).

## Organisation

- **`src/`** –  C++ benchmarks
- **`data/`** – CSV samples.
- **`python/`** – Plotting scripts
- **`plots/`** – The resulting graphs.

## Benchmarks 

### 0. CPU specification.
Below the Cache sizes and CPU specifications I used:

<img src="./plots/CPU-Z.png" width="300">

### 1. Cache hierarchy latency
We walk a random‑order pointer chain to avoid the prefetcher and measure L1/L2/L3 reads.


Plotting only the red line at first, I was a bit confused. Why are access times for L3 cache not uniform? Given the range $2^{13}$ KB (8MB) to $2^{14}$ KB (16MB) matches the TLB size, I suspected these are due to TLB misses.
Using 4KB pages and around 3000-4000 entries in L2 TLB, we can only map 12 MB of memory. So I pinned huge pages (2GB) in memory and tried again:

<img src="./plots/cache_sweep_comparison.png" width="800">


### 2. Branch prediction & speculation
Compares sorted vs. shuffled data to force real branch instructions. 

Modeling Note: Modern compilers often attempt to use CMOV (Conditional Move) to avoid branches.
I used benchmark::DoNotOptimize within conditional blocks to avoid this.
![](.\plots\branch_prediction.png)
### 3. L3 latency & TLB thrashing
Every single memory access is timed with `RDTSC` + `_mm_lfence`.  
<img src="./plots/tlb_sweep.png" width="800">
### 4. 3D V‑Cache performance
AMD's patent for "Balanced Latency stacked cache" (https://patents.google.com/patent/US20260003794A1/en), claims near‑perfect, single‑spike latency distribution due to the 9800X3D’s “flipped” design (CCD on top of the cache).
I checked and it looks like they are right! (We can see this in the above cache sweep image too.)
<img src="./plots/l3_latency_distribution.png" width="800">
### 5. False Sharing 
Measuring the impact of false sharing, not much to say here.

### 6. Instruction Level parallelism (ILP) & Dependency Chains
Two benchmarks, ILP_Dependent tests raw latency of CPU's ALUs by making each instruction wait for the previous one and ILP_Independent to see the speedup using Out-of-order execution and ILP.
From what I read, the Zen 5 architecture should have 6 ALUs.


## Build & run

**Requirements**
- C++23 compiler (GCC 13+ or Clang 16+)
- CMake 3.31+
- Google Benchmark
- Python 3.10+ with `pandas`, `matplotlib`, `seaborn`

**Build**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .