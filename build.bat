@echo off
REM Build script for Sudoku Solver (Windows - no make required)

echo ========================================
echo Building Sudoku Solver Project
echo ========================================
echo.

REM Create directories
if not exist bin mkdir bin
if not exist puzzles mkdir puzzles
if not exist results mkdir results


echo [1/3] Compiling basic sudoku solver...
gcc -fopenmp -O3 -Wall -march=native -ffast-math -o bin\sudoku.exe sudoku.c
if %errorlevel% neq 0 (
    echo ERROR: Failed to compile sudoku.c
    pause
    exit /b 1
)
echo ✓ bin\sudoku.exe created

echo [2/3] Compiling advanced sudoku solver...
gcc -fopenmp -O3 -Wall -march=native -ffast-math -o bin\sudoku_advanced.exe sudoku_advanced.c
if %errorlevel% neq 0 (
    echo ERROR: Failed to compile sudoku_advanced.c
    pause
    exit /b 1
)
echo ✓ bin\sudoku_advanced.exe created

echo [3/3] Compiling benchmark tool...
gcc -fopenmp -O3 -Wall -march=native -ffast-math -o bin\sudoku_benchmark.exe benchmark.c
if %errorlevel% neq 0 (
    echo ERROR: Failed to compile benchmark.c
    pause
    exit /b 1
)
echo ✓ bin\sudoku_benchmark.exe created

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo Executables created in 'bin/' folder:
echo   - bin\sudoku.exe (basic solver)
echo   - bin\sudoku_advanced.exe (advanced solver)
echo   - bin\sudoku_benchmark.exe (benchmarking tool)
echo.
echo Next steps:
echo   1. Generate puzzles: bin\sudoku_advanced.exe generate
echo   2. Run tests: run_tests.bat
echo   3. Analyze results: python analyze_results.py
echo.
pause
