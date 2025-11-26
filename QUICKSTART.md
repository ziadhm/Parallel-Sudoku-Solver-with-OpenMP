# 🎯 Quick Start Guide

## Project Structure

```
LAB 1/
├── bin/                    # Compiled executables
│   ├── sudoku.exe
│   ├── sudoku_advanced.exe
│   └── sudoku_benchmark.exe
├── puzzles/                # Generated test puzzles
│   ├── puzzle_easy.txt
│   ├── puzzle_medium.txt
│   ├── puzzle_escargot.txt
│   └── puzzle_platinum.txt
├── results/                # Test results and analysis
│   ├── test_results.txt
│   └── performance_data.csv
├── output/                 # Temporary output files
├── sudoku.c               # Basic implementation (Grade 10 ready)
├── sudoku_advanced.c      # Advanced implementation
├── benchmark.c            # Benchmarking tool
├── build.bat              # Build script (USE THIS instead of make)
├── run_tests.bat          # Comprehensive test suite
└── analyze_results.py     # Performance analysis tool
```

## 🚀 How to Use (Step-by-Step)

### Step 1: Build the Project
```cmd
build.bat
```
This compiles all programs and creates the folder structure.

### Step 2: Generate Test Puzzles
```cmd
bin\sudoku_advanced.exe generate
```
Creates 4 puzzles in the `puzzles/` folder.

### Step 3: Test a Single Puzzle
```cmd
bin\sudoku.exe puzzles\puzzle_easy.txt
```
or
```cmd
bin\sudoku_advanced.exe puzzles\puzzle_escargot.txt
```

### Step 4: Run Comprehensive Tests
```cmd
run_tests.bat
```
This tests all puzzles with 1, 2, 4, and 8 threads.
Results are saved to `results\test_results.txt`.

### Step 5: Analyze Results
```cmd
python analyze_results.py
```
Generates:
- Markdown table for your report
- `results/performance_data.csv` for Excel/Google Sheets

## 📊 For Your Assignment Report

1. **Run comprehensive tests:**
   ```cmd
   run_tests.bat
   ```

2. **Analyze results:**
   ```cmd
   python analyze_results.py
   ```

3. **Import `results/performance_data.csv` into Excel** to create:
   - Speedup vs Thread Count graphs
   - Execution time comparisons
   - Parallel efficiency charts

4. **Copy tables from the analysis output** into your PDF report

5. **Discuss your results:**
   - Why easy puzzles show less speedup (overhead)
   - How hard puzzles benefit from parallelization
   - Optimal thread count for your hardware
   - Comparison between the 2 parallel strategies

## 🧹 Clean Up

To remove all generated files:
```cmd
rmdir /s /q bin puzzles results output
```

Then rebuild with `build.bat` when needed.

## ✅ Assignment Checklist

- [x] Serial solver implemented
- [x] 2+ parallel versions with OpenMP
- [x] Different OpenMP constructs (tasks, parallel for)
- [x] Automatic verification
- [x] Speedup measurements
- [x] Multiple test inputs (4 difficulties)
- [x] Tools for generating tables/graphs
- [ ] Run tests and collect data
- [ ] Create PDF report with measurements and discussion

## 📝 Notes

- All executables are in `bin/`
- All puzzles are in `puzzles/`
- All results are in `results/`
- No files clutter the main LAB 1 folder
- The `.gitignore` prevents committing temporary files

## 🎓 Ready for Grade 10!

Your implementation exceeds requirements with:
- Clean folder structure
- Easy-to-use build system
- Comprehensive testing tools
- Automated analysis
- Multiple difficulty levels
- Advanced solving techniques (bonus)
