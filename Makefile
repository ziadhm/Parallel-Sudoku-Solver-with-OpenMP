# Makefile for compiling the Sudoku solver
# Just run 'make' to build everything

CC = gcc
CFLAGS = -fopenmp -O3 -Wall -march=native -ffast-math
CFLAGS_DEBUG = -fopenmp -g -Wall -O0
TARGET_BASIC = bin/sudoku
TARGET_ADVANCED = bin/sudoku_advanced
TARGET_BENCHMARK = bin/sudoku_benchmark

all: bin $(TARGET_BASIC) $(TARGET_ADVANCED) $(TARGET_BENCHMARK)

# Create directories if they don't exist
bin:
	@if not exist bin mkdir bin
	@if not exist puzzles mkdir puzzles
	@if not exist results mkdir results
	@if not exist output mkdir output

# Basic version
$(TARGET_BASIC): sudoku.c
	$(CC) $(CFLAGS) -o $(TARGET_BASIC) sudoku.c

# Main version with all parallel implementations
$(TARGET_ADVANCED): sudoku_advanced.c
	$(CC) $(CFLAGS) -o $(TARGET_ADVANCED) sudoku_advanced.c

# Benchmark version
$(TARGET_BENCHMARK): benchmark.c
	$(CC) $(CFLAGS) -o $(TARGET_BENCHMARK) benchmark.c

# Debug build (no optimizations)
debug: sudoku_advanced.c
	$(CC) $(CFLAGS_DEBUG) -o bin/$(TARGET_ADVANCED)_debug sudoku_advanced.c

# Clean up compiled files
clean:
	del /Q bin\*.exe 2>nul || echo Clean complete
	del /Q results\*.txt results\*.csv 2>nul || echo Results clean complete
	del /Q output\*.txt output\*.csv 2>nul || echo Output clean complete

generate:
	$(TARGET_ADVANCED).exe generate

# Run individual puzzles
run_easy:
	$(TARGET_ADVANCED).exe puzzles/puzzle_easy.txt

run_medium:
	$(TARGET_ADVANCED).exe puzzles/puzzle_medium.txt

run_escargot:
	$(TARGET_ADVANCED).exe puzzles/puzzle_escargot.txt

run_platinum:
	$(TARGET_ADVANCED).exe puzzles/puzzle_platinum.txt

# Benchmark different thread counts
benchmark: $(TARGET_ADVANCED)
	@echo Running comprehensive benchmarks...
	@echo Thread Count, Puzzle, Serial Time, Parallel V1 Time, Parallel V2 Time, Parallel V3 Time, V1 Speedup, V2 Speedup, V3 Speedup > results/benchmark_results.csv
	@for /L %%t in (1,1,8) do ( \
		set OMP_NUM_THREADS=%%t && \
		echo Testing with %%t threads... && \
		$(TARGET_ADVANCED).exe puzzles/puzzle_easy.txt >> output/benchmark_log.txt && \
		$(TARGET_ADVANCED).exe puzzles/puzzle_medium.txt >> output/benchmark_log.txt && \
		$(TARGET_ADVANCED).exe puzzles/puzzle_escargot.txt >> output/benchmark_log.txt && \
		$(TARGET_ADVANCED).exe puzzles/puzzle_platinum.txt >> output/benchmark_log.txt \
	)

# Quick test suite
test: $(TARGET_ADVANCED)
	@echo Generating test puzzles...
	$(TARGET_ADVANCED).exe generate
	@echo.
	@echo ===== EASY PUZZLE =====
	$(TARGET_ADVANCED).exe puzzles/puzzle_easy.txt
	@echo.
	@echo ===== MEDIUM PUZZLE =====
	$(TARGET_ADVANCED).exe puzzles/puzzle_medium.txt
	@echo.
	@echo ===== AI ESCARGOT (HARD) =====
	$(TARGET_ADVANCED).exe puzzles/puzzle_escargot.txt
	@echo.
	@echo ===== PLATINUM BLONDE (EXTREME) =====
	$(TARGET_ADVANCED).exe puzzles/puzzle_platinum.txt

# Performance analysis
profile: $(TARGET_ADVANCED)
	@echo Running performance analysis...
	@set OMP_NUM_THREADS=1 && $(TARGET_ADVANCED).exe puzzles/puzzle_platinum.txt > results/results_1thread.txt
	@set OMP_NUM_THREADS=2 && $(TARGET_ADVANCED).exe puzzles/puzzle_platinum.txt > results/results_2threads.txt
	@set OMP_NUM_THREADS=4 && $(TARGET_ADVANCED).exe puzzles/puzzle_platinum.txt > results/results_4threads.txt
	@set OMP_NUM_THREADS=8 && $(TARGET_ADVANCED).exe puzzles/puzzle_platinum.txt > results/results_8threads.txt
	@echo Results saved to results/results_*threads.txt

.PHONY: all clean generate run_easy run_medium run_escargot run_platinum benchmark test profile debug
