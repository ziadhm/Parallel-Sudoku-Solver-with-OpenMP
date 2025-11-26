#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define N 9
#define UNASSIGNED 0

// Function prototypes
int solve_serial(int grid[N][N]);
int solve_parallel_v1(int grid[N][N]);
int solve_parallel_v2(int grid[N][N]);
int is_valid(int grid[N][N], int row, int col, int num);
int find_empty_location(int grid[N][N], int *row, int *col);
void print_grid(int grid[N][N]);
void copy_grid(int src[N][N], int dst[N][N]);
int verify_solution(int grid[N][N]);
void load_puzzle(const char *filename, int grid[N][N]);
void generate_test_puzzles();

// Serial backtracking solver
int solve_serial(int grid[N][N]) {
    int row, col;
    
    if (!find_empty_location(grid, &row, &col))
        return 1; // Success - puzzle solved
    
    for (int num = 1; num <= 9; num++) {
        if (is_valid(grid, row, col, num)) {
            grid[row][col] = num;
            
            if (solve_serial(grid))
                return 1;
            
            grid[row][col] = UNASSIGNED; // Backtrack
        }
    }
    
    return 0; // Trigger backtracking
}

// Parallel Version 1: Using OpenMP tasks for recursive branching
int solve_parallel_v1_helper(int grid[N][N], int depth) {
    int row, col;
    
    if (!find_empty_location(grid, &row, &col))
        return 1; // Success
    
    int solved = 0;
    
    // Create tasks for different numbers at shallow depth
    if (depth < 2) { // Limit task creation depth
        #pragma omp parallel
        {
            #pragma omp single
            {
                for (int num = 1; num <= 9 && !solved; num++) {
                    if (is_valid(grid, row, col, num)) {
                        #pragma omp task shared(solved) firstprivate(num, row, col)
                        {
                            if (!solved) {
                                int local_grid[N][N];
                                #pragma omp critical
                                copy_grid(grid, local_grid);
                                
                                local_grid[row][col] = num;
                                
                                if (solve_parallel_v1_helper(local_grid, depth + 1)) {
                                    #pragma omp critical
                                    {
                                        if (!solved) {
                                            copy_grid(local_grid, grid);
                                            solved = 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                #pragma omp taskwait
            }
        }
    } else {
        // Use serial solving for deeper levels
        for (int num = 1; num <= 9; num++) {
            if (is_valid(grid, row, col, num)) {
                grid[row][col] = num;
                
                if (solve_serial(grid))
                    return 1;
                
                grid[row][col] = UNASSIGNED;
            }
        }
    }
    
    return solved;
}

int solve_parallel_v1(int grid[N][N]) {
    return solve_parallel_v1_helper(grid, 0);
}

// Parallel Version 2: Using OpenMP parallel for with speculation
int solve_parallel_v2(int grid[N][N]) {
    int row, col;
    
    if (!find_empty_location(grid, &row, &col))
        return 1; // Already solved
    
    int solved = 0;
    int solution_grid[N][N];
    
    #pragma omp parallel shared(solved, solution_grid)
    {
        #pragma omp for schedule(dynamic)
        for (int num = 1; num <= 9; num++) {
            if (!solved && is_valid(grid, row, col, num)) {
                int local_grid[N][N];
                copy_grid(grid, local_grid);
                local_grid[row][col] = num;
                
                if (solve_serial(local_grid)) {
                    #pragma omp critical
                    {
                        if (!solved) {
                            copy_grid(local_grid, solution_grid);
                            solved = 1;
                        }
                    }
                }
            }
        }
    }
    
    if (solved) {
        copy_grid(solution_grid, grid);
        return 1;
    }
    
    return 0;
}

// Check if num is valid at grid[row][col]
int is_valid(int grid[N][N], int row, int col, int num) {
    // Check row
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num)
            return 0;
    
    // Check column
    for (int x = 0; x < N; x++)
        if (grid[x][col] == num)
            return 0;
    
    // Check 3x3 box
    int start_row = row - row % 3;
    int start_col = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + start_row][j + start_col] == num)
                return 0;
    
    return 1;
}

// Find an empty location in the grid
int find_empty_location(int grid[N][N], int *row, int *col) {
    for (*row = 0; *row < N; (*row)++)
        for (*col = 0; *col < N; (*col)++)
            if (grid[*row][*col] == UNASSIGNED)
                return 1;
    return 0;
}

// Print the grid
void print_grid(int grid[N][N]) {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++)
            printf("%2d ", grid[row][col]);
        printf("\n");
    }
}

// Copy grid
void copy_grid(int src[N][N], int dst[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            dst[i][j] = src[i][j];
}

// Verify that the solution is correct
int verify_solution(int grid[N][N]) {
    // Check all rows
    for (int i = 0; i < N; i++) {
        int used[N + 1] = {0};
        for (int j = 0; j < N; j++) {
            int num = grid[i][j];
            if (num < 1 || num > 9 || used[num])
                return 0;
            used[num] = 1;
        }
    }
    
    // Check all columns
    for (int j = 0; j < N; j++) {
        int used[N + 1] = {0};
        for (int i = 0; i < N; i++) {
            int num = grid[i][j];
            if (num < 1 || num > 9 || used[num])
                return 0;
            used[num] = 1;
        }
    }
    
    // Check all 3x3 boxes
    for (int box_row = 0; box_row < 3; box_row++) {
        for (int box_col = 0; box_col < 3; box_col++) {
            int used[N + 1] = {0};
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int num = grid[box_row * 3 + i][box_col * 3 + j];
                    if (num < 1 || num > 9 || used[num])
                        return 0;
                    used[num] = 1;
                }
            }
        }
    }
    
    return 1;
}

// Load puzzle from file
void load_puzzle(const char *filename, int grid[N][N]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fscanf(file, "%d", &grid[i][j]) != 1) {
                printf("Error: Invalid puzzle format\n");
                fclose(file);
                exit(1);
            }
        }
    }
    
    fclose(file);
}

// Generate test puzzles with different difficulties
void generate_test_puzzles() {
    // Easy puzzle (45 clues)
    int easy[N][N] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    // Medium puzzle (30 clues)
    int medium[N][N] = {
        {0, 0, 0, 2, 6, 0, 7, 0, 1},
        {6, 8, 0, 0, 7, 0, 0, 9, 0},
        {1, 9, 0, 0, 0, 4, 5, 0, 0},
        {8, 2, 0, 1, 0, 0, 0, 4, 0},
        {0, 0, 4, 6, 0, 2, 9, 0, 0},
        {0, 5, 0, 0, 0, 3, 0, 2, 8},
        {0, 0, 9, 3, 0, 0, 0, 7, 4},
        {0, 4, 0, 0, 5, 0, 0, 3, 6},
        {7, 0, 3, 0, 1, 8, 0, 0, 0}
    };
    
    // Hard puzzle (25 clues)
    int hard[N][N] = {
        {0, 0, 0, 6, 0, 0, 4, 0, 0},
        {7, 0, 0, 0, 0, 3, 6, 0, 0},
        {0, 0, 0, 0, 9, 1, 0, 8, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 5, 0, 1, 8, 0, 0, 0, 3},
        {0, 0, 0, 3, 0, 6, 0, 4, 5},
        {0, 4, 0, 2, 0, 0, 0, 6, 0},
        {9, 0, 3, 0, 0, 0, 0, 0, 0},
        {0, 2, 0, 0, 0, 0, 1, 0, 0}
    };
    
    FILE *f;
    
    f = fopen("puzzles/puzzle_easy.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            fprintf(f, "%d ", easy[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    
    f = fopen("puzzles/puzzle_medium.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            fprintf(f, "%d ", medium[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    
    f = fopen("puzzles/puzzle_hard.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            fprintf(f, "%d ", hard[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    
    printf("Generated test puzzles in 'puzzles/' folder:\n");
    printf("  - puzzles/puzzle_easy.txt\n");
    printf("  - puzzles/puzzle_medium.txt\n");
    printf("  - puzzles/puzzle_hard.txt\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <puzzle_file> [generate]\n", argv[0]);
        printf("   Or: %s generate  (to generate test puzzles)\n", argv[0]);
        return 1;
    }
    
    // Generate test puzzles if requested
    if (strcmp(argv[1], "generate") == 0) {
        generate_test_puzzles();
        return 0;
    }
    
    int grid[N][N];
    load_puzzle(argv[1], grid);
    
    printf("Original Puzzle:\n");
    print_grid(grid);
    printf("\n");
    
    // Test Serial Version
    int grid_serial[N][N];
    copy_grid(grid, grid_serial);
    
    double start = omp_get_wtime();
    int result_serial = solve_serial(grid_serial);
    double time_serial = omp_get_wtime() - start;
    
    printf("=== SERIAL VERSION ===\n");
    if (result_serial) {
        printf("Solution found!\n");
        if (verify_solution(grid_serial)) {
            printf("✓ Solution verified as CORRECT\n");
        } else {
            printf("✗ Solution verification FAILED\n");
        }
        print_grid(grid_serial);
    } else {
        printf("No solution exists\n");
    }
    printf("Time: %.6f seconds\n\n", time_serial);
    
    // Test Parallel Version 1 (Tasks)
    int grid_parallel1[N][N];
    copy_grid(grid, grid_parallel1);
    
    start = omp_get_wtime();
    int result_parallel1 = solve_parallel_v1(grid_parallel1);
    double time_parallel1 = omp_get_wtime() - start;
    
    printf("=== PARALLEL VERSION 1 (OpenMP Tasks) ===\n");
    if (result_parallel1) {
        printf("Solution found!\n");
        if (verify_solution(grid_parallel1)) {
            printf("✓ Solution verified as CORRECT\n");
        } else {
            printf("✗ Solution verification FAILED\n");
        }
        print_grid(grid_parallel1);
    } else {
        printf("No solution exists\n");
    }
    printf("Time: %.6f seconds\n", time_parallel1);
    printf("Speedup: %.2fx\n\n", time_serial / time_parallel1);
    
    // Test Parallel Version 2 (Parallel For)
    int grid_parallel2[N][N];
    copy_grid(grid, grid_parallel2);
    
    start = omp_get_wtime();
    int result_parallel2 = solve_parallel_v2(grid_parallel2);
    double time_parallel2 = omp_get_wtime() - start;
    
    printf("=== PARALLEL VERSION 2 (OpenMP Parallel For) ===\n");
    if (result_parallel2) {
        printf("Solution found!\n");
        if (verify_solution(grid_parallel2)) {
            printf("✓ Solution verified as CORRECT\n");
        } else {
            printf("✗ Solution verification FAILED\n");
        }
        print_grid(grid_parallel2);
    } else {
        printf("No solution exists\n");
    }
    printf("Time: %.6f seconds\n", time_parallel2);
    printf("Speedup: %.2fx\n\n", time_serial / time_parallel2);
    
    // Summary
    printf("=== SUMMARY ===\n");
    printf("Serial:      %.6f seconds\n", time_serial);
    printf("Parallel V1: %.6f seconds (%.2fx speedup)\n", time_parallel1, time_serial / time_parallel1);
    printf("Parallel V2: %.6f seconds (%.2fx speedup)\n", time_parallel2, time_serial / time_parallel2);
    
    return 0;
}
