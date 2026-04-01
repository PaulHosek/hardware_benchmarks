import os
import pandas as pd
import matplotlib.pyplot as plt

script_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.join(script_dir, '..', 'data', 'tlb_sweep.csv')
data_path = os.path.normpath(data_path)

df = pd.read_csv(data_path)
df['size_mb'] = df['size_bytes'] / (1024 * 1024)
df['num_pages'] = df['size_bytes'] / 4096

plt.figure(figsize=(11, 6))
plt.plot(df['size_mb'], df['cycles_per_access'], marker='o', linestyle='-', color='darkblue', markersize=4)

plt.xscale('log', base=2)
plt.title('TLB Latency vs Working Set Size (Ryzen 9800X3D example)')
plt.xlabel('Working Set Size (MB) — Log₂ Scale')
plt.ylabel('Latency (Clock Cycles per Access)')
plt.grid(True, which="both", ls="--", alpha=0.5)

plt.axvline(x=0.25, color='gray', linestyle=':', label='L1 DTLB (~64 entries ≈ 256 KB)')
plt.axvline(x=4, color='gray', linestyle='--', label='L2 TLB (~1K entries ≈ 4 MB)')
plt.axvline(x=16, color='black', linestyle='-.', label='L2 TLB extended (~4K entries ≈ 16 MB)')
plt.axvline(x=64, color='purple', linestyle='-', label='Shared TLB / page-walk region')

plt.legend()
plt.tight_layout()
plt.savefig("../plots/tlb_sweep.png")