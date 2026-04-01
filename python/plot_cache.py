import os
import pandas as pd
import matplotlib.pyplot as plt

script_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.join(script_dir, '..', 'data', 'cache_sweep.csv')
data_path = os.path.normpath(data_path)

df = pd.read_csv(data_path)

df['size_kb'] = df['size_bytes'] / 1024

plt.figure(figsize=(10, 6))
plt.plot(df['size_kb'], df['cycles_per_access'], marker='o', linestyle='-', color='crimson')
plt.xscale('log', base=2)

plt.title('Memory Latency vs. Working Set Size (Ryzen 9800X3D)')
plt.xlabel('Working Set Size (KB) - Log2 Scale')
plt.ylabel('Latency (Clock Cycles per Access)')
plt.grid(True, which="both", ls="--", alpha=0.5)

plt.axvline(x=32, color='gray', linestyle=':', label='L1 Boundary (32KB)')
plt.axvline(x=1024, color='gray', linestyle='--', label='L2 Boundary (1MB)')
plt.axvline(x=98304, color='black', linestyle='-.', label='L3 V-Cache Boundary (~96MB)')

plt.legend()
plt.tight_layout()

plt.savefig("../plots/cache_sweep.png")


