import os
import pandas as pd
import matplotlib.pyplot as plt

script_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.normpath(os.path.join(script_dir, '..', 'data', 'cache_sweep.csv'))

if not os.path.exists(data_path):
    print(f"Error: Could not find {data_path}")
    exit()

df = pd.read_csv(data_path)

df['size_kb'] = df['size_bytes'] / 1024

plt.figure(figsize=(12, 7))

plt.plot(df['size_kb'], df['cycles_std'],
         label="Standard (4KB) Pages",
         marker='.', linestyle='-', color='crimson', alpha=0.7)

df_large = df[df['cycles_large'] > 0].copy()
if not df_large.empty:
    plt.plot(df_large['size_kb'], df_large['cycles_large'],
             label="Large (2MB) Pages",
             marker='o', linestyle='-', color='dodgerblue', linewidth=2)
else:
    print("Warning: No Large Page data found in CSV. Did you restart after the SecPol change?")

plt.xscale('log', base=2)
plt.title('Memory Latency: Ryzen 7 9800X3D (4KB vs 2MB Pages)')
plt.xlabel('Working Set Size (KB)')
plt.ylabel('Latency (Cycles per Access)')

plt.axvline(x=48, color='green', linestyle=':', alpha=0.6, label='L1 Data (48KB)')
plt.axvline(x=1024, color='orange', linestyle='--', alpha=0.6, label='L2 (1MB)')
plt.axvline(x=96 * 1024, color='purple', linestyle='-.', alpha=0.8, label='L3 V-Cache (96MB)')

plt.grid(True, which="both", ls="--", alpha=0.3)
plt.legend()
plt.tight_layout()

save_path = os.path.normpath(os.path.join(script_dir, '..', 'plots', 'cache_sweep_comparison.png'))
os.makedirs(os.path.dirname(save_path), exist_ok=True)
plt.savefig(save_path, dpi=300)
plt.show()