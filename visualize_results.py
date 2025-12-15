#!/usr/bin/env python3
# Graph generator for my Sudoku project
# Creates 4 different graphs from the CSV data

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load the performance data
df = pd.read_csv('results/performance_data.csv')

# Make it look decent
sns.set_style("whitegrid")
plt.rcParams['figure.figsize'] = (12, 8)

# Setup a 2x2 grid of graphs
fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Sudoku Solver Performance Analysis', fontsize=16, fontweight='bold')

# Graph 1: How speedup changes with thread count
ax1 = axes[0, 0]
for puzzle in df['Puzzle'].unique():
    puzzle_data = df[df['Puzzle'] == puzzle]
    ax1.plot(puzzle_data['Threads'], puzzle_data['V1 Speedup'], 
             marker='o', label=puzzle, linewidth=2)
ax1.set_xlabel('Thread Count')
ax1.set_ylabel('Speedup (V1 - Tasks)')
ax1.set_title('Speedup vs Thread Count (Parallel V1)')
ax1.legend()
ax1.grid(True, alpha=0.3)

# Graph 2: Compare the three parallel approaches
ax2 = axes[0, 1]
puzzles = ['AI Escargot', 'Platinum']  # just the hard ones
x = df[df['Puzzle'].isin(puzzles)].groupby('Threads').size().index
width = 0.25
for i, version in enumerate(['V1 Speedup', 'V2 Speedup', 'V3 Speedup']):
    means = df[df['Puzzle'].isin(puzzles)].groupby('Threads')[version].mean()
    ax2.bar([xi + i*width for xi in range(len(x))], means, 
            width, label=f'Parallel {version[:2]}')
ax2.set_xlabel('Thread Count')
ax2.set_ylabel('Average Speedup')
ax2.set_title('Parallel Strategy Comparison (Hard Puzzles)')
ax2.set_xticks([xi + width for xi in range(len(x))])
ax2.set_xticklabels(x)
ax2.legend()
ax2.grid(True, alpha=0.3, axis='y')

# Graph 3: Show difficulty by backtrack count
ax3 = axes[1, 0]
backtrack_data = df[df['Threads'] == 1][['Puzzle', 'Backtracks']]
colors = ['green', 'yellow', 'orange', 'red']
bars = ax3.bar(backtrack_data['Puzzle'], backtrack_data['Backtracks'], color=colors)
ax3.set_xlabel('Puzzle')
ax3.set_ylabel('Number of Backtracks')
ax3.set_title('Puzzle Difficulty (Backtrack Count)')
ax3.tick_params(axis='x', rotation=45)
# Add numbers on top of bars
for bar in bars:
    height = bar.get_height()
    ax3.text(bar.get_x() + bar.get_width()/2., height,
             f'{int(height)}', ha='center', va='bottom')

# Graph 4: Heatmap showing everything at once
ax4 = axes[1, 1]
pivot = df.pivot_table(values='V1 Speedup', index='Puzzle', columns='Threads')
sns.heatmap(pivot, annot=True, fmt='.2f', cmap='YlOrRd', ax=ax4, cbar_kws={'label': 'Speedup'})
ax4.set_title('Speedup Heatmap (Parallel V1)')
ax4.set_xlabel('Thread Count')
ax4.set_ylabel('Puzzle')

plt.tight_layout()
plt.savefig('results/performance_graphs.png', dpi=300, bbox_inches='tight')
print("✓ Graphs saved to: results/performance_graphs.png")
print("\nOpening the graph...")
plt.show()
