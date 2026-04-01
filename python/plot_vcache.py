import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

script_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.join(script_dir, '..', 'data', 'vcache_samples.csv')
data_path = os.path.normpath(data_path)

df = pd.read_csv(data_path)


# df_filtered = df[(df['cycles'] > 30) & (df['cycles'] < 150)]
# Filter out the 90+ cycle TLB misses and focus ONLY on pure L3 hits
df_filtered = df[(df['cycles'] > 40) & (df['cycles'] < 60)]

# Increase the bins so we can see the micro-variance inside this tight range
sns.histplot(df_filtered['cycles'], bins=20, kde=True, color='royalblue')
plt.figure(figsize=(12, 6))
sns.histplot(df_filtered['cycles'], bins=120, kde=False, color='royalblue')

plt.title('AMD Ryzen 7 9800X3D L3 Latency Distribution (80MB Working Set)')
plt.xlabel('Clock Cycles')
plt.ylabel('Frequency')
plt.grid(True, alpha=0.3)

plt.savefig("../plots/l3_latency_distribution.png")