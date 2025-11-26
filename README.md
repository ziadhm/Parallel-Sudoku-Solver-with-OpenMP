# Sudoku Solver - OpenMP Parallel Implementation

## Assignment: OpenMP Parallel Programming
**Course:** Parallel Algorithms  
**Variant:** Sudoku Puzzle Solver

## Project Structure

```
LAB 1/
├── sudoku.c          # Main implementation (serial + 2 parallel versions)
├── Makefile          # Build configuration
├── README.md         # This file
├── puzzle_easy.txt   # Easy difficulty puzzle (generated)
├── puzzle_medium.txt # Medium difficulty puzzle (generated)
└── puzzle_hard.txt   # Hard difficulty puzzle (generated)
```

## Implementation Overview

This project implements a Sudoku solver with three versions:

1. **Serial Version**: Classic backtracking algorithm
2. **Parallel Version 1**: OpenMP tasks for recursive branching
3. **Parallel Version 2**: OpenMP parallel for with dynamic scheduling

All solutions are automatically verified for correctness.

## Compilation

### Build the project:
```bash
make
```

### Build with specific optimization:
```bash
gcc -fopenmp -O2 -Wall -o sudoku sudoku.c
```

## Usage

### Generate test puzzles:
```bash
sudoku.exe generate
```

### Solve a specific puzzle:
```bash
sudoku.exe puzzle_easy.txt
sudoku.exe puzzle_medium.txt
sudoku.exe puzzle_hard.txt
```

### Run all tests:
```bash
make test
```

## Makefile Targets

- `make` or `make all` - Compile the program
- `make clean` - Remove executable
- `make run_generate` - Generate test puzzles
- `make run_easy` - Run easy puzzle
- `make run_medium` - Run medium puzzle
- `make run_hard` - Run hard puzzle
- `make test` - Generate puzzles and run all tests

## Puzzle Format

Puzzles are stored in text files with 9 rows and 9 columns.
Use `0` for empty cells and `1-9` for filled cells.

Example:
```
5 3 0 0 7 0 0 0 0
6 0 0 1 9 5 0 0 0
0 9 8 0 0 0 0 6 0
...
```

## Parallel Strategies

### Version 1: OpenMP Tasks
- Creates parallel tasks for different number choices at shallow recursion depths
- Uses task synchronization with `taskwait`
- Switches to serial solving at deeper recursion levels for efficiency

### Version 2: OpenMP Parallel For
- Parallelizes the first level of number choices
- Uses dynamic scheduling for load balancing
- Each thread works on a separate grid copy to avoid race conditions

## Performance Measurement

The program automatically measures and reports:
- Execution time for each version
- Speedup compared to serial version
- Solution verification status

## Expected Output

```
Original Puzzle:
 5  3  0  0  7  0  0  0  0
 6  0  0  1  9  5  0  0  0
...

=== SERIAL VERSION ===
Solution found!
✓ Solution verified as CORRECT
 5  3  4  6  7  8  9  1  2
...
Time: 0.002345 seconds

=== PARALLEL VERSION 1 (OpenMP Tasks) ===
Solution found!
✓ Solution verified as CORRECT
...
Time: 0.001234 seconds
Speedup: 1.90x

=== PARALLEL VERSION 2 (OpenMP Parallel For) ===
Solution found!
✓ Solution verified as CORRECT
...
Time: 0.001456 seconds
Speedup: 1.61x

=== SUMMARY ===
Serial:      0.002345 seconds
Parallel V1: 0.001234 seconds (1.90x speedup)
Parallel V2: 0.001456 seconds (1.61x speedup)
```

## Testing & Verification

- All solutions are automatically verified using constraint checking
- Tests three difficulty levels: Easy (45 clues), Medium (30 clues), Hard (25 clues)
- Ensures parallel versions produce identical correct results

## Notes for Measurements

For your assignment report, consider:

1. **Different puzzle difficulties**: Easy, Medium, Hard
2. **Thread scaling**: Test with different `OMP_NUM_THREADS` values
   ```bash
   set OMP_NUM_THREADS=1
   sudoku.exe puzzle_hard.txt
   
   set OMP_NUM_THREADS=4
   sudoku.exe puzzle_hard.txt
   ```
3. **Multiple runs**: Average results over 5-10 runs for statistical significance
4. **System specs**: Document your CPU, cores, RAM

## OpenMP Configuration

Set number of threads:
```bash
set OMP_NUM_THREADS=4
```

## Requirements Met

✓ Serial implementation  
✓ Two different parallel versions using different OpenMP constructs  
✓ Automatic solution verification  
✓ Performance measurements (time, speedup)  
✓ Makefile for compilation  
✓ Multiple test cases with different difficulties  
✓ Ready for measurement and analysis

## Author

Created for PA Lab Assignment 1 - OpenMP Parallel Programming

## License

Educational use only




Quick Commands:
# Compile
gcc -fopenmp -O2 -Wall -o sudoku sudoku.c

# Generate puzzles
sudoku.exe generate

# Run tests
sudoku.exe puzzle_easy.txt
sudoku.exe puzzle_medium.txt
sudoku.exe puzzle_hard.txt

For Your Report:
Test with different thread counts:
set OMP_NUM_THREADS=1
sudoku.exe puzzle_hard.txt

set OMP_NUM_THREADS=2
sudoku.exe puzzle_hard.txt

set OMP_NUM_THREADS=4
sudoku.exe puzzle_hard.txt


2. Run Comprehensive Tests:
# Test with different thread counts
set OMP_NUM_THREADS=1
sudoku.exe puzzle_easy.txt > results_1thread_easy.txt
sudoku.exe puzzle_medium.txt > results_1thread_medium.txt
sudoku.exe puzzle_hard.txt > results_1thread_hard.txt

set OMP_NUM_THREADS=2
sudoku.exe puzzle_easy.txt > results_2thread_easy.txt
sudoku.exe puzzle_medium.txt > results_2thread_medium.txt
sudoku.exe puzzle_hard.txt > results_2thread_hard.txt

set OMP_NUM_THREADS=4
sudoku.exe puzzle_easy.txt > results_4thread_easy.txt
sudoku.exe puzzle_medium.txt > results_4thread_medium.txt
sudoku.exe puzzle_hard.txt > results_4thread_hard.txt

set OMP_NUM_THREADS=8
sudoku.exe puzzle_easy.txt > results_8thread_easy.txt
sudoku.exe puzzle_medium.txt > results_8thread_medium.txt
sudoku.exe puzzle_hard.txt > results_8thread_hard.txt