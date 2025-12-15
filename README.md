# Parallel Sudoku Solver with OpenMP

A high-performance Sudoku solver implementation comparing serial and parallel approaches using OpenMP. This project explores different parallelization strategies and analyzes their performance characteristics.

## Features

- **Multiple Parallel Implementations**: Three different OpenMP-based parallel approaches
- **Performance Analysis**: Comprehensive benchmarking and visualization tools
- **Optimized Serial Baseline**: Efficient constraint propagation with backtracking
- **Automated Testing**: Complete test suite with multiple thread configurations

## Project Structure

- `sudoku_advanced.c` - Main solver with 3 parallel implementations
- `sudoku.c` - Serial implementation baseline
- `benchmark.c` - Performance benchmarking utilities
- `run_tests.bat` - Automated test suite runner
- `analyze_results.py` - Results parser and statistical analysis
- `visualize_results.py` - Performance visualization generator

## Prerequisites

### Compiler
- GCC with OpenMP support (MinGW-w64 on Windows)
- MSVC with OpenMP support (alternative for Windows)

### Python Dependencies
```bash
pip install pandas matplotlib seaborn
```

## Building

### Using Make (Linux/WSL/MinGW)
```bash
make
```

### Using Build Script (Windows)
```cmd
build.bat
```

## Running

### Execute Test Suite
```bash
# Windows
run_tests.bat

# Linux/WSL
./run_tests.sh
```

The test suite runs benchmarks with 1, 2, 4, and 8 threads across various puzzle difficulties.

### Generate Analysis and Visualizations
```bash
python analyze_results.py
python visualize_results.py
```

### View Results
```bash
# Windows
start report.html

# Linux
xdg-open report.html
```

## Parallel Implementations

### Version 1: Task-Based Parallelism
Uses OpenMP task directives with dynamic work stealing for load balancing. Best suited for irregular workloads where puzzle complexity varies significantly.

### Version 2: Parallel For Loops
Employs OpenMP parallel for constructs to distribute search space iterations. More structured approach with lower overhead but less flexible load balancing.

### Version 3: Hybrid Approach
Dynamically selects between task-based and parallel for strategies based on puzzle difficulty metrics, attempting to leverage strengths of both approaches.

## Performance Characteristics

Key findings from the benchmarking analysis:

- **Parallel Overhead**: Simple puzzles show slowdown due to parallelization overhead exceeding computation time
- **Scalability**: Complex puzzles (e.g., AI Escargot) demonstrate measurable speedup with multiple threads
- **Algorithm Trade-offs**: Task-based approach generally outperforms parallel for due to better load balancing
- **Problem Size**: 9×9 Sudoku grid limits parallelization benefits; constraint propagation solves most cells before backtracking

## Output Files

After running the test suite, the following files are generated in the `results/` directory:

- `test_results.txt` - Raw benchmark output from all test runs
- `performance_data.csv` - Structured data for analysis
- `performance_graphs.png` - Performance comparison visualizations

## Technical Notes

- Timing precision: Millisecond resolution (very fast solves may show 0.000000 seconds)
- Solution verification: All parallel implementations produce identical correct solutions
- Thread scaling: Tested with 1, 2, 4, and 8 threads
- Puzzle set: Includes easy, medium, hard, and expert-level puzzles

## Lessons Learned

- Parallelization introduces overhead that must be justified by computational workload
- Problem size significantly impacts parallel efficiency
- Effective load balancing strategies are crucial for irregular workloads
- Algorithm optimizations can reduce parallelization opportunities

## License

This project is available for educational purposes.

---

*Parallel Programming with OpenMP - 2025*
