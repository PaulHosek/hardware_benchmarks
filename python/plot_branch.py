import os
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

script_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.join(script_dir, '..', 'data', 'branch_data.csv')
data_path = os.path.normpath(data_path)

df = pd.read_csv(data_path)

plt.figure(figsize=(8, 6))
sns.boxplot(x='condition', y='cycles', data=df, palette='Set2')
sns.stripplot(x='condition', y='cycles', data=df, color='black', alpha=0.5, jitter=True)

plt.title('Branch Predictor Penalty (100k array elements)')
plt.xlabel('Data State')
plt.ylabel('Execution Time (Clock Cycles)')
plt.grid(axis='y', linestyle='--', alpha=0.7)

plt.tight_layout()
plt.savefig("../plots/branch_prediction.png")