@echo off
REM Test script - runs the solver with different thread counts
REM This takes a while to complete

echo ========================================
echo Sudoku Solver Test Suite
echo ========================================
echo.

REM First generate the test puzzles if they don't exist
echo [1/5] Generating test puzzles...
bin\sudoku_advanced.exe generate
echo.

REM Run with 1 thread first
echo [2/5] Testing with 1 thread...
set OMP_NUM_THREADS=1
echo === 1 THREAD === > results\test_results.txt
echo. >> results\test_results.txt
echo Easy Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_easy.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Medium Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_medium.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo AI Escargot: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_escargot.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Platinum Blonde: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_platinum.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo.

REM Test with 2 threads
echo [3/5] Testing with 2 threads...
set OMP_NUM_THREADS=2
echo === 2 THREADS === >> results\test_results.txt
echo. >> results\test_results.txt
echo Easy Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_easy.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Medium Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_medium.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo AI Escargot: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_escargot.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Platinum Blonde: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_platinum.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo.

REM Test with 4 threads
echo [4/5] Testing with 4 threads...
set OMP_NUM_THREADS=4
echo === 4 THREADS === >> results\test_results.txt
echo. >> results\test_results.txt
echo Easy Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_easy.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Medium Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_medium.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo AI Escargot: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_escargot.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Platinum Blonde: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_platinum.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo.

REM Test with 8 threads
echo [5/5] Testing with 8 threads...
set OMP_NUM_THREADS=8
echo === 8 THREADS === >> results\test_results.txt
echo. >> results\test_results.txt
echo Easy Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_easy.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Medium Puzzle: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_medium.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo AI Escargot: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_escargot.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo Platinum Blonde: >> results\test_results.txt
bin\sudoku_advanced.exe puzzles\puzzle_platinum.txt >> results\test_results.txt
echo. >> results\test_results.txt
echo.

echo ========================================
echo Testing Complete!
echo Results saved to: results\test_results.txt
echo ========================================
echo.

REM Display final results
echo Summary:
echo - 4 puzzle difficulties tested
echo - 4 thread configurations (1, 2, 4, 8)
echo - 3 parallel versions compared
echo - Full results in results\test_results.txt
echo.
echo Next steps for your report:
echo 1. Run: python analyze_results.py
echo 2. Check results\performance_data.csv for graphs
echo 3. Create tables and speedup graphs
echo 4. Discuss results in your PDF
echo.
pause
