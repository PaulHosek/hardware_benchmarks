import os
import pandas as pd
import matplotlib.pyplot as plt

script_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.normpath(os.path.join(script_dir, '..', 'data', 'ilp_sweep.csv'))

if not os.path.exists(data_path):
    print(f"Error: Could not find {data_path}")
    exit()

df = pd.read_csv(data_path)

plt.figure(figsize=(10, 6))
plt.plot(df['chains'], df['cycles_per_iteration'],
         marker='o', linestyle='-', color='teal', linewidth=2.5, markersize=8)

plt.title('Instruction Level Parallelism (Ryzen 7 9800X3D)', fontsize=14)
plt.xlabel('Number of Independent Execution Chains', fontsize=12)
plt.ylabel('Total Cycles per Iteration', fontsize=12)
plt.xticks(range(1, 13))

plt.grid(True, linestyle='--', alpha=0.6)

plt.axvspan(1, 6, color='lightgreen', alpha=0.2, label='Zen 5 ALU Saturation Zone (1-6)')

plt.legend()
plt.tight_layout()

plt.savefig("../plots/ilp_sweep.png")