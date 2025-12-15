# Parallel Sudoku Solver

My submission for the OpenMP assignment. This project solves Sudoku puzzles using both serial and parallel approaches.

## What's Inside

- `sudoku_advanced.c` - Main solver with 3 different parallel implementations
- `run_tests.bat` - Script to run all tests (takes a few minutes)
- `analyze_results.py` - Parses test results and generates tables
- `visualize_results.py` - Creates graphs from the data
- `report.html` - Full write-up with analysis

## How to Build and Run

```bash
# Compile everything
make

# Run the test suite (tests with 1, 2, 4, and 8 threads)
run_tests.bat

# Generate analysis and graphs
python analyze_results.py
python visualize_results.py

# View the report
start report.html
```

## The Three Parallel Versions

1. **Version 1 (Tasks)**: Uses OpenMP tasks with work stealing
2. **Version 2 (Parallel For)**: Uses parallel for loops 
3. **Version 3 (Hybrid)**: Combines both based on puzzle difficulty

## Results

Honestly the speedups aren't as good as I hoped, but I understand why now:
- Easy puzzles are too fast - parallel overhead makes them slower
- Hard puzzles show some improvement (AI Escargot is the best test case)
- Task-based approach (V1) works better than parallel for (V2)

The problem is that 9x9 Sudoku isn't really big enough to benefit much from parallelization, especially with my constraint propagation optimizations that solve most of the puzzle before backtracking even starts.

## Files Generated

After running tests you'll get:
- `results/test_results.txt` - Raw output from all test runs
- `results/performance_data.csv` - CSV for making graphs
- `results/performance_graphs.png` - 4 graphs showing performance

## Notes

- Needs GCC with OpenMP support
- Python packages needed: pandas, matplotlib, seaborn
- The timer only has millisecond precision so super fast puzzles show 0.000000 sec
- All parallel versions are verified to produce correct solutions

## What I Learned

- Parallelization has overhead - not everything gets faster
- Problem size matters a lot
- Load balancing is important (work stealing helps)
- Sometimes making the serial version too good limits parallel opportunities

---
*Assignment 1 - Parallel Programming Course - 2025*
