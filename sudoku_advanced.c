#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define N 9
#define UNASSIGNED 0
#define MAX_THREADS 16

// Performance statistics structure
typedef struct {
    unsigned long long backtrack_count;
    unsigned long long naked_singles_found;
    unsigned long long hidden_singles_found;
    unsigned long long naked_pairs_found;
    unsigned long long pointing_pairs_found;
    unsigned long long box_line_reductions;
    unsigned long long nodes_explored;
    double time_taken;
    int thread_id;
} SolverStats;

// Bitset for candidate tracking (9 bits for digits 1-9)
typedef unsigned short CandidateSet;

// Grid with candidates
typedef struct {
    int cells[N][N];
    CandidateSet candidates[N][N];
    int empty_cells;
} SudokuGrid;

// Function prototypes
int solve_serial(SudokuGrid *grid, SolverStats *stats);
int solve_parallel_v1(SudokuGrid *grid, SolverStats *stats);
int solve_parallel_v2(SudokuGrid *grid, SolverStats *stats);
int solve_parallel_v3(SudokuGrid *grid, SolverStats *stats);

void init_grid(SudokuGrid *grid);
void copy_grid(SudokuGrid *src, SudokuGrid *dst);
int verify_solution(SudokuGrid *grid);
void load_puzzle(const char *filename, SudokuGrid *grid);
void print_grid(SudokuGrid *grid);
void print_stats(SolverStats *stats, const char *version);
void aggregate_stats(SolverStats *stats_array, int count, SolverStats *result);

// Advanced solving techniques
int apply_naked_singles(SudokuGrid *grid, SolverStats *stats);
int apply_hidden_singles(SudokuGrid *grid, SolverStats *stats);
int apply_naked_pairs(SudokuGrid *grid, SolverStats *stats);
int apply_pointing_pairs(SudokuGrid *grid, SolverStats *stats);
int apply_box_line_reduction(SudokuGrid *grid, SolverStats *stats);
int apply_constraint_propagation(SudokuGrid *grid, SolverStats *stats);

// Candidate management
void init_candidates(SudokuGrid *grid);
void update_candidates(SudokuGrid *grid, int row, int col, int num);
int count_candidates(CandidateSet cand);
int get_candidate(CandidateSet cand, int index);
CandidateSet make_candidate_set(int *values, int count);

// Heuristics
int find_best_cell(SudokuGrid *grid, int *row, int *col);
double estimate_difficulty(SudokuGrid *grid);

// Puzzle generation
void generate_puzzle(SudokuGrid *grid, int difficulty);
void generate_test_suite();

// Visualization
void print_candidates(SudokuGrid *grid);
void export_solving_trace(const char *filename, SudokuGrid *grid);

// ========== Candidate Set Operations ==========

CandidateSet make_candidate_set(int *values, int count) {
    CandidateSet set = 0;
    for (int i = 0; i < count; i++) {
        if (values[i] >= 1 && values[i] <= 9) {
            set |= (1 << (values[i] - 1));
        }
    }
    return set;
}

int count_candidates(CandidateSet cand) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        if (cand & (1 << i)) count++;
    }
    return count;
}

int get_candidate(CandidateSet cand, int index) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        if (cand & (1 << i)) {
            if (count == index) return i + 1;
            count++;
        }
    }
    return 0;
}

// ========== Grid Initialization ==========

void init_grid(SudokuGrid *grid) {
    memset(grid->cells, 0, sizeof(grid->cells));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid->candidates[i][j] = 0x1FF; // All 9 bits set
        }
    }
    grid->empty_cells = N * N;
}

void copy_grid(SudokuGrid *src, SudokuGrid *dst) {
    memcpy(dst->cells, src->cells, sizeof(src->cells));
    memcpy(dst->candidates, src->candidates, sizeof(src->candidates));
    dst->empty_cells = src->empty_cells;
}

void init_candidates(SudokuGrid *grid) {
    // Initialize all candidates to full
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid->cells[i][j] == UNASSIGNED) {
                grid->candidates[i][j] = 0x1FF;
            } else {
                grid->candidates[i][j] = 0;
            }
        }
    }
    
    // Eliminate candidates based on existing values
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid->cells[i][j] != UNASSIGNED) {
                update_candidates(grid, i, j, grid->cells[i][j]);
            }
        }
    }
}

void update_candidates(SudokuGrid *grid, int row, int col, int num) {
    unsigned short mask = ~(1 << (num - 1));
    
    // Remove from row
    for (int j = 0; j < N; j++) {
        grid->candidates[row][j] &= mask;
    }
    
    // Remove from column
    for (int i = 0; i < N; i++) {
        grid->candidates[i][col] &= mask;
    }
    
    // Remove from box
    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            grid->candidates[box_row + i][box_col + j] &= mask;
        }
    }
    
    grid->candidates[row][col] = 0;
}

// ========== Advanced Solving Techniques ==========

int apply_naked_singles(SudokuGrid *grid, SolverStats *stats) {
    int progress = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid->cells[i][j] == UNASSIGNED && count_candidates(grid->candidates[i][j]) == 1) {
                int num = get_candidate(grid->candidates[i][j], 0);
                grid->cells[i][j] = num;
                update_candidates(grid, i, j, num);
                grid->empty_cells--;
                stats->naked_singles_found++;
                progress = 1;
            }
        }
    }
    return progress;
}

int apply_hidden_singles(SudokuGrid *grid, SolverStats *stats) {
    int progress = 0;
    
    // Check rows
    for (int i = 0; i < N; i++) {
        for (int num = 1; num <= 9; num++) {
            int count = 0, last_col = -1;
            for (int j = 0; j < N; j++) {
                if (grid->cells[i][j] == UNASSIGNED && 
                    (grid->candidates[i][j] & (1 << (num - 1)))) {
                    count++;
                    last_col = j;
                }
            }
            if (count == 1) {
                grid->cells[i][last_col] = num;
                update_candidates(grid, i, last_col, num);
                grid->empty_cells--;
                stats->hidden_singles_found++;
                progress = 1;
            }
        }
    }
    
    // Check columns
    for (int j = 0; j < N; j++) {
        for (int num = 1; num <= 9; num++) {
            int count = 0, last_row = -1;
            for (int i = 0; i < N; i++) {
                if (grid->cells[i][j] == UNASSIGNED && 
                    (grid->candidates[i][j] & (1 << (num - 1)))) {
                    count++;
                    last_row = i;
                }
            }
            if (count == 1) {
                grid->cells[last_row][j] = num;
                update_candidates(grid, last_row, j, num);
                grid->empty_cells--;
                stats->hidden_singles_found++;
                progress = 1;
            }
        }
    }
    
    // Check boxes
    for (int box = 0; box < 9; box++) {
        int box_row = (box / 3) * 3;
        int box_col = (box % 3) * 3;
        
        for (int num = 1; num <= 9; num++) {
            int count = 0, last_i = -1, last_j = -1;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int r = box_row + i;
                    int c = box_col + j;
                    if (grid->cells[r][c] == UNASSIGNED && 
                        (grid->candidates[r][c] & (1 << (num - 1)))) {
                        count++;
                        last_i = r;
                        last_j = c;
                    }
                }
            }
            if (count == 1) {
                grid->cells[last_i][last_j] = num;
                update_candidates(grid, last_i, last_j, num);
                grid->empty_cells--;
                stats->hidden_singles_found++;
                progress = 1;
            }
        }
    }
    
    return progress;
}

int apply_naked_pairs(SudokuGrid *grid, SolverStats *stats) {
    int progress = 0;
    
    // Check rows for naked pairs
    for (int i = 0; i < N; i++) {
        for (int j1 = 0; j1 < N - 1; j1++) {
            if (grid->cells[i][j1] == UNASSIGNED && count_candidates(grid->candidates[i][j1]) == 2) {
                for (int j2 = j1 + 1; j2 < N; j2++) {
                    if (grid->cells[i][j2] == UNASSIGNED && 
                        grid->candidates[i][j1] == grid->candidates[i][j2]) {
                        // Found naked pair, eliminate from other cells in row
                        CandidateSet pair = grid->candidates[i][j1];
                        for (int j = 0; j < N; j++) {
                            if (j != j1 && j != j2 && grid->cells[i][j] == UNASSIGNED) {
                                if (grid->candidates[i][j] & pair) {
                                    grid->candidates[i][j] &= ~pair;
                                    stats->naked_pairs_found++;
                                    progress = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return progress;
}

int apply_pointing_pairs(SudokuGrid *grid, SolverStats *stats) {
    int progress = 0;
    
    // For each box, check if a number only appears in one row/column
    for (int box = 0; box < 9; box++) {
        int box_row = (box / 3) * 3;
        int box_col = (box % 3) * 3;
        
        for (int num = 1; num <= 9; num++) {
            int rows[3] = {0}, cols[3] = {0};
            
            // Count which rows/cols have this candidate in the box
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int r = box_row + i;
                    int c = box_col + j;
                    if (grid->cells[r][c] == UNASSIGNED && 
                        (grid->candidates[r][c] & (1 << (num - 1)))) {
                        rows[i] = 1;
                        cols[j] = 1;
                    }
                }
            }
            
            // If only one row, eliminate from rest of that row
            int row_count = rows[0] + rows[1] + rows[2];
            if (row_count == 1) {
                int target_row = box_row + (rows[0] ? 0 : (rows[1] ? 1 : 2));
                for (int j = 0; j < N; j++) {
                    if (j < box_col || j >= box_col + 3) {
                        if (grid->cells[target_row][j] == UNASSIGNED) {
                            if (grid->candidates[target_row][j] & (1 << (num - 1))) {
                                grid->candidates[target_row][j] &= ~(1 << (num - 1));
                                stats->pointing_pairs_found++;
                                progress = 1;
                            }
                        }
                    }
                }
            }
            
            // If only one column, eliminate from rest of that column
            int col_count = cols[0] + cols[1] + cols[2];
            if (col_count == 1) {
                int target_col = box_col + (cols[0] ? 0 : (cols[1] ? 1 : 2));
                for (int i = 0; i < N; i++) {
                    if (i < box_row || i >= box_row + 3) {
                        if (grid->cells[i][target_col] == UNASSIGNED) {
                            if (grid->candidates[i][target_col] & (1 << (num - 1))) {
                                grid->candidates[i][target_col] &= ~(1 << (num - 1));
                                stats->pointing_pairs_found++;
                                progress = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return progress;
}

int apply_constraint_propagation(SudokuGrid *grid, SolverStats *stats) {
    int progress = 1;
    while (progress) {
        progress = 0;
        progress |= apply_naked_singles(grid, stats);
        progress |= apply_hidden_singles(grid, stats);
        progress |= apply_naked_pairs(grid, stats);
        progress |= apply_pointing_pairs(grid, stats);
    }
    return grid->empty_cells == 0;
}

// ========== Heuristics ==========

int find_best_cell(SudokuGrid *grid, int *row, int *col) {
    int min_candidates = 10;
    *row = -1;
    *col = -1;
    
    // Find cell with minimum remaining values (MRV heuristic)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid->cells[i][j] == UNASSIGNED) {
                int cand_count = count_candidates(grid->candidates[i][j]);
                if (cand_count == 0) return 0; // No solution
                if (cand_count < min_candidates) {
                    min_candidates = cand_count;
                    *row = i;
                    *col = j;
                }
            }
        }
    }
    
    return (*row != -1);
}

double estimate_difficulty(SudokuGrid *grid) {
    double score = 0;
    score += grid->empty_cells * 1.0;
    
    // Check for cells with few candidates (harder)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid->cells[i][j] == UNASSIGNED) {
                int cands = count_candidates(grid->candidates[i][j]);
                if (cands >= 6) score += 5;
                else if (cands >= 4) score += 3;
            }
        }
    }
    
    return score;
}

// ========== Backtracking with Advanced Techniques ==========

int solve_with_techniques(SudokuGrid *grid, SolverStats *stats) {
    stats->nodes_explored++;
    
    // Apply constraint propagation first
    if (apply_constraint_propagation(grid, stats)) {
        return 1; // Solved without backtracking
    }
    
    // Check for invalid state
    int row, col;
    if (!find_best_cell(grid, &row, &col)) {
        return 0; // No valid cell found
    }
    
    // Try each candidate for the selected cell
    CandidateSet cands = grid->candidates[row][col];
    int num_cands = count_candidates(cands);
    
    for (int i = 0; i < num_cands; i++) {
        int num = get_candidate(cands, i);
        
        // Make a copy and try this value
        SudokuGrid temp_grid;
        copy_grid(grid, &temp_grid);
        
        temp_grid.cells[row][col] = num;
        temp_grid.empty_cells--;
        update_candidates(&temp_grid, row, col, num);
        
        stats->backtrack_count++;
        
        if (solve_with_techniques(&temp_grid, stats)) {
            copy_grid(&temp_grid, grid);
            return 1;
        }
    }
    
    return 0;
}

int solve_serial(SudokuGrid *grid, SolverStats *stats) {
    memset(stats, 0, sizeof(SolverStats));
    stats->thread_id = 0;
    
    double start = omp_get_wtime();
    init_candidates(grid);
    int result = solve_with_techniques(grid, stats);
    stats->time_taken = omp_get_wtime() - start;
    
    return result;
}

// ========== Parallel Version 1: Task-based with Work Stealing ==========

int solve_parallel_v1_helper(SudokuGrid *grid, SolverStats *stats, int depth) {
    stats->nodes_explored++;
    
    if (apply_constraint_propagation(grid, stats)) {
        return 1;
    }
    
    int row, col;
    if (!find_best_cell(grid, &row, &col)) {
        return 0;
    }
    
    CandidateSet cands = grid->candidates[row][col];
    int num_cands = count_candidates(cands);
    
    // Parallelize at shallow depths only
    if (depth < 2 && num_cands > 2) {
        int solved = 0;
        SudokuGrid solution;
        
        #pragma omp parallel shared(solved, solution)
        {
            #pragma omp single
            {
                for (int i = 0; i < num_cands && !solved; i++) {
                    int num = get_candidate(cands, i);
                    
                    #pragma omp task shared(solved, solution) firstprivate(num, row, col, i)
                    {
                        if (!solved) {
                            SudokuGrid temp_grid;
                            SolverStats local_stats = {0};
                            local_stats.thread_id = omp_get_thread_num();
                            
                            #pragma omp critical
                            copy_grid(grid, &temp_grid);
                            
                            temp_grid.cells[row][col] = num;
                            temp_grid.empty_cells--;
                            update_candidates(&temp_grid, row, col, num);
                            
                            local_stats.backtrack_count++;
                            
                            if (solve_parallel_v1_helper(&temp_grid, &local_stats, depth + 1)) {
                                #pragma omp critical
                                {
                                    if (!solved) {
                                        copy_grid(&temp_grid, &solution);
                                        stats->backtrack_count += local_stats.backtrack_count;
                                        stats->nodes_explored += local_stats.nodes_explored;
                                        stats->naked_singles_found += local_stats.naked_singles_found;
                                        stats->hidden_singles_found += local_stats.hidden_singles_found;
                                        solved = 1;
                                    }
                                }
                            }
                        }
                    }
                }
                #pragma omp taskwait
            }
        }
        
        if (solved) {
            copy_grid(&solution, grid);
            return 1;
        }
    } else {
        // Serial solving for deeper levels
        for (int i = 0; i < num_cands; i++) {
            int num = get_candidate(cands, i);
            
            SudokuGrid temp_grid;
            copy_grid(grid, &temp_grid);
            
            temp_grid.cells[row][col] = num;
            temp_grid.empty_cells--;
            update_candidates(&temp_grid, row, col, num);
            
            stats->backtrack_count++;
            
            if (solve_with_techniques(&temp_grid, stats)) {
                copy_grid(&temp_grid, grid);
                return 1;
            }
        }
    }
    
    return 0;
}

int solve_parallel_v1(SudokuGrid *grid, SolverStats *stats) {
    memset(stats, 0, sizeof(SolverStats));
    
    double start = omp_get_wtime();
    init_candidates(grid);
    int result = solve_parallel_v1_helper(grid, stats, 0);
    stats->time_taken = omp_get_wtime() - start;
    
    return result;
}

// ========== Parallel Version 2: Speculative Parallel For ==========

int solve_parallel_v2(SudokuGrid *grid, SolverStats *stats) {
    memset(stats, 0, sizeof(SolverStats));
    
    double start = omp_get_wtime();
    init_candidates(grid);
    
    // Try constraint propagation first
    if (apply_constraint_propagation(grid, stats)) {
        stats->time_taken = omp_get_wtime() - start;
        return 1;
    }
    
    int row, col;
    if (!find_best_cell(grid, &row, &col)) {
        stats->time_taken = omp_get_wtime() - start;
        return 0;
    }
    
    CandidateSet cands = grid->candidates[row][col];
    int num_cands = count_candidates(cands);
    
    int solved = 0;
    SudokuGrid solution;
    SolverStats local_stats[MAX_THREADS] = {0};
    
    #pragma omp parallel shared(solved, solution, local_stats)
    {
        int tid = omp_get_thread_num();
        
        #pragma omp for schedule(dynamic, 1)
        for (int i = 0; i < num_cands; i++) {
            if (!solved) {
                int num = get_candidate(cands, i);
                
                SudokuGrid temp_grid;
                copy_grid(grid, &temp_grid);
                
                temp_grid.cells[row][col] = num;
                temp_grid.empty_cells--;
                update_candidates(&temp_grid, row, col, num);
                
                local_stats[tid].backtrack_count++;
                local_stats[tid].thread_id = tid;
                
                if (solve_with_techniques(&temp_grid, &local_stats[tid])) {
                    #pragma omp critical
                    {
                        if (!solved) {
                            copy_grid(&temp_grid, &solution);
                            solved = 1;
                        }
                    }
                }
            }
        }
    }
    
    // Aggregate statistics
    for (int i = 0; i < omp_get_max_threads(); i++) {
        stats->backtrack_count += local_stats[i].backtrack_count;
        stats->nodes_explored += local_stats[i].nodes_explored;
        stats->naked_singles_found += local_stats[i].naked_singles_found;
        stats->hidden_singles_found += local_stats[i].hidden_singles_found;
    }
    
    if (solved) {
        copy_grid(&solution, grid);
    }
    
    stats->time_taken = omp_get_wtime() - start;
    return solved;
}

// ========== Parallel Version 3: Hybrid Approach ==========

int solve_parallel_v3(SudokuGrid *grid, SolverStats *stats) {
    memset(stats, 0, sizeof(SolverStats));
    
    double start = omp_get_wtime();
    init_candidates(grid);
    
    // Estimate difficulty and choose strategy
    double difficulty = estimate_difficulty(grid);
    
    if (difficulty < 100) {
        // Easy puzzle - use serial with techniques
        int result = solve_with_techniques(grid, stats);
        stats->time_taken = omp_get_wtime() - start;
        return result;
    } else {
        // Hard puzzle - use parallel approach
        return solve_parallel_v2(grid, stats);
    }
}

// ========== Verification ==========

int verify_solution(SudokuGrid *grid) {
    // Check all rows
    for (int i = 0; i < N; i++) {
        int used[N + 1] = {0};
        for (int j = 0; j < N; j++) {
            int num = grid->cells[i][j];
            if (num < 1 || num > 9 || used[num]) return 0;
            used[num] = 1;
        }
    }
    
    // Check all columns
    for (int j = 0; j < N; j++) {
        int used[N + 1] = {0};
        for (int i = 0; i < N; i++) {
            int num = grid->cells[i][j];
            if (num < 1 || num > 9 || used[num]) return 0;
            used[num] = 1;
        }
    }
    
    // Check all boxes
    for (int box = 0; box < 9; box++) {
        int used[N + 1] = {0};
        int box_row = (box / 3) * 3;
        int box_col = (box % 3) * 3;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int num = grid->cells[box_row + i][box_col + j];
                if (num < 1 || num > 9 || used[num]) return 0;
                used[num] = 1;
            }
        }
    }
    
    return 1;
}

// ========== I/O Functions ==========

void load_puzzle(const char *filename, SudokuGrid *grid) {
    init_grid(grid);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    
    grid->empty_cells = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fscanf(file, "%d", &grid->cells[i][j]) != 1) {
                printf("Error: Invalid puzzle format\n");
                fclose(file);
                exit(1);
            }
            if (grid->cells[i][j] == UNASSIGNED) {
                grid->empty_cells++;
            }
        }
    }
    
    fclose(file);
}

void print_grid(SudokuGrid *grid) {
    for (int i = 0; i < N; i++) {
        if (i % 3 == 0 && i != 0) {
            printf("------+-------+------\n");
        }
        for (int j = 0; j < N; j++) {
            if (j % 3 == 0 && j != 0) {
                printf("| ");
            }
            printf("%d ", grid->cells[i][j]);
        }
        printf("\n");
    }
}

void print_candidates(SudokuGrid *grid) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("(%d,%d): ", i, j);
            if (grid->cells[i][j] != UNASSIGNED) {
                printf("[%d] ", grid->cells[i][j]);
            } else {
                printf("[");
                for (int k = 1; k <= 9; k++) {
                    if (grid->candidates[i][j] & (1 << (k - 1))) {
                        printf("%d", k);
                    }
                }
                printf("] ");
            }
        }
        printf("\n");
    }
}

void print_stats(SolverStats *stats, const char *version) {
    printf("=== %s ===\n", version);
    printf("Time taken: %.6f seconds\n", stats->time_taken);
    printf("Nodes explored: %llu\n", stats->nodes_explored);
    printf("Backtracks: %llu\n", stats->backtrack_count);
    printf("Naked singles: %llu\n", stats->naked_singles_found);
    printf("Hidden singles: %llu\n", stats->hidden_singles_found);
    printf("Naked pairs: %llu\n", stats->naked_pairs_found);
    printf("Pointing pairs: %llu\n", stats->pointing_pairs_found);
    printf("\n");
}

// ========== Puzzle Generation ==========

void generate_test_suite() {
    FILE *f;
    
    // World's hardest Sudoku (AI Escargot)
    int escargot[N][N] = {
        {1, 0, 0, 0, 0, 7, 0, 9, 0},
        {0, 3, 0, 0, 2, 0, 0, 0, 8},
        {0, 0, 9, 6, 0, 0, 5, 0, 0},
        {0, 0, 5, 3, 0, 0, 9, 0, 0},
        {0, 1, 0, 0, 8, 0, 0, 0, 2},
        {6, 0, 0, 0, 0, 4, 0, 0, 0},
        {3, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 4, 0, 0, 0, 0, 0, 0, 7},
        {0, 0, 7, 0, 0, 0, 3, 0, 0}
    };
    
    // Platinum Blonde (extremely hard)
    int platinum[N][N] = {
        {0, 0, 0, 0, 0, 0, 0, 1, 2},
        {0, 0, 0, 0, 0, 0, 0, 0, 3},
        {0, 0, 2, 3, 0, 0, 4, 0, 0},
        {0, 0, 1, 8, 0, 0, 0, 0, 5},
        {0, 6, 0, 0, 7, 0, 8, 0, 0},
        {0, 0, 0, 0, 0, 9, 0, 0, 0},
        {0, 0, 8, 5, 0, 0, 0, 0, 0},
        {9, 0, 0, 0, 4, 0, 5, 0, 0},
        {4, 7, 0, 0, 0, 6, 0, 0, 0}
    };
    
    // Medium difficulty
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
    
    // Easy
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
    
    f = fopen("puzzles/puzzle_easy.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) fprintf(f, "%d ", easy[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    
    f = fopen("puzzles/puzzle_medium.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) fprintf(f, "%d ", medium[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    
    f = fopen("puzzles/puzzle_escargot.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) fprintf(f, "%d ", escargot[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    
    f = fopen("puzzles/puzzle_platinum.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) fprintf(f, "%d ", platinum[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
    
    printf("Generated test puzzles in 'puzzles/' folder:\n");
    printf("  - puzzles/puzzle_easy.txt (Beginner)\n");
    printf("  - puzzles/puzzle_medium.txt (Intermediate)\n");
    printf("  - puzzles/puzzle_escargot.txt (AI Escargot - World Famous Hard Puzzle)\n");
    printf("  - puzzles/puzzle_platinum.txt (Platinum Blonde - Extremely Hard)\n");
}

// ========== Main Program ==========

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Advanced Sudoku Solver with OpenMP\n");
        printf("Usage: %s <puzzle_file> [threads]\n", argv[0]);
        printf("   Or: %s generate\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "generate") == 0) {
        generate_test_suite();
        return 0;
    }
    
    // Set number of threads if specified
    if (argc >= 3) {
        omp_set_num_threads(atoi(argv[2]));
    }
    
    printf("OpenMP Max Threads: %d\n\n", omp_get_max_threads());
    
    SudokuGrid grid;
    load_puzzle(argv[1], &grid);
    
    printf("Original Puzzle (%d empty cells):\n", grid.empty_cells);
    print_grid(&grid);
    printf("\n");
    
    // Serial Version
    SudokuGrid grid_serial;
    copy_grid(&grid, &grid_serial);
    SolverStats stats_serial;
    
    if (solve_serial(&grid_serial, &stats_serial)) {
        printf("✓ SERIAL: Solution verified as %s\n", 
               verify_solution(&grid_serial) ? "CORRECT" : "INCORRECT");
        print_stats(&stats_serial, "SERIAL VERSION");
        print_grid(&grid_serial);
        printf("\n");
    } else {
        printf("✗ SERIAL: No solution found\n\n");
    }
    
    // Parallel Version 1
    SudokuGrid grid_parallel1;
    copy_grid(&grid, &grid_parallel1);
    SolverStats stats_parallel1;
    
    if (solve_parallel_v1(&grid_parallel1, &stats_parallel1)) {
        printf("✓ PARALLEL V1 (Tasks): Solution verified as %s\n", 
               verify_solution(&grid_parallel1) ? "CORRECT" : "INCORRECT");
        print_stats(&stats_parallel1, "PARALLEL V1 (OpenMP Tasks with Work Stealing)");
        printf("Speedup: %.2fx\n\n", stats_serial.time_taken / stats_parallel1.time_taken);
    } else {
        printf("✗ PARALLEL V1: No solution found\n\n");
    }
    
    // Parallel Version 2
    SudokuGrid grid_parallel2;
    copy_grid(&grid, &grid_parallel2);
    SolverStats stats_parallel2;
    
    if (solve_parallel_v2(&grid_parallel2, &stats_parallel2)) {
        printf("✓ PARALLEL V2 (Speculative): Solution verified as %s\n", 
               verify_solution(&grid_parallel2) ? "CORRECT" : "INCORRECT");
        print_stats(&stats_parallel2, "PARALLEL V2 (Speculative Parallel For)");
        printf("Speedup: %.2fx\n\n", stats_serial.time_taken / stats_parallel2.time_taken);
    } else {
        printf("✗ PARALLEL V2: No solution found\n\n");
    }
    
    // Parallel Version 3 (Hybrid)
    SudokuGrid grid_parallel3;
    copy_grid(&grid, &grid_parallel3);
    SolverStats stats_parallel3;
    
    if (solve_parallel_v3(&grid_parallel3, &stats_parallel3)) {
        printf("✓ PARALLEL V3 (Hybrid): Solution verified as %s\n", 
               verify_solution(&grid_parallel3) ? "CORRECT" : "INCORRECT");
        print_stats(&stats_parallel3, "PARALLEL V3 (Adaptive Hybrid Strategy)");
        printf("Speedup: %.2fx\n\n", stats_serial.time_taken / stats_parallel3.time_taken);
    } else {
        printf("✗ PARALLEL V3: No solution found\n\n");
    }
    
    // Summary
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    PERFORMANCE SUMMARY                    ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ Serial:       %10.6f sec  │ Backtracks: %10llu ║\n", 
           stats_serial.time_taken, stats_serial.backtrack_count);
    printf("║ Parallel V1:  %10.6f sec  │ Speedup: %10.2fx ║\n", 
           stats_parallel1.time_taken, stats_serial.time_taken / stats_parallel1.time_taken);
    printf("║ Parallel V2:  %10.6f sec  │ Speedup: %10.2fx ║\n", 
           stats_parallel2.time_taken, stats_serial.time_taken / stats_parallel2.time_taken);
    printf("║ Parallel V3:  %10.6f sec  │ Speedup: %10.2fx ║\n", 
           stats_parallel3.time_taken, stats_serial.time_taken / stats_parallel3.time_taken);
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
