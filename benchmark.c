#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N 9
#define NUM_RUNS 5
#define MAX_THREADS 16

typedef struct {
    double serial_time;
    double parallel_v1_time;
    double parallel_v2_time;
    double parallel_v3_time;
} BenchmarkResult;

void run_benchmark(const char *puzzle_file, const char *puzzle_name, int num_threads, FILE *output) {
    char command[512];
    BenchmarkResult results[NUM_RUNS];
    
    printf("Benchmarking %s with %d threads (%d runs)...\n", puzzle_name, num_threads, NUM_RUNS);
    
    for (int run = 0; run < NUM_RUNS; run++) {
        sprintf(command, "set OMP_NUM_THREADS=%d && sudoku_advanced.exe %s", num_threads, puzzle_file);
        
        // Execute and parse results (simplified - you'd need to capture output properly)
        FILE *fp = _popen(command, "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, "Serial:")) {
                    sscanf(line, "║ Serial: %lf", &results[run].serial_time);
                } else if (strstr(line, "Parallel V1:")) {
                    sscanf(line, "║ Parallel V1: %lf", &results[run].parallel_v1_time);
                } else if (strstr(line, "Parallel V2:")) {
                    sscanf(line, "║ Parallel V2: %lf", &results[run].parallel_v2_time);
                } else if (strstr(line, "Parallel V3:")) {
                    sscanf(line, "║ Parallel V3: %lf", &results[run].parallel_v3_time);
                }
            }
            _pclose(fp);
        }
    }
    
    // Calculate averages
    double avg_serial = 0, avg_v1 = 0, avg_v2 = 0, avg_v3 = 0;
    for (int i = 0; i < NUM_RUNS; i++) {
        avg_serial += results[i].serial_time;
        avg_v1 += results[i].parallel_v1_time;
        avg_v2 += results[i].parallel_v2_time;
        avg_v3 += results[i].parallel_v3_time;
    }
    avg_serial /= NUM_RUNS;
    avg_v1 /= NUM_RUNS;
    avg_v2 /= NUM_RUNS;
    avg_v3 /= NUM_RUNS;
    
    // Write to CSV
    fprintf(output, "%d,%s,%.6f,%.6f,%.6f,%.6f,%.2f,%.2f,%.2f\n",
            num_threads, puzzle_name,
            avg_serial, avg_v1, avg_v2, avg_v3,
            avg_serial / avg_v1, avg_serial / avg_v2, avg_serial / avg_v3);
    
    printf("  Average times: Serial=%.6fs, V1=%.6fs (%.2fx), V2=%.6fs (%.2fx), V3=%.6fs (%.2fx)\n\n",
           avg_serial, avg_v1, avg_serial/avg_v1, avg_v2, avg_serial/avg_v2, avg_v3, avg_serial/avg_v3);
}

int main() {
    const char *puzzles[] = {
        "puzzle_easy.txt",
        "puzzle_medium.txt",
        "puzzle_escargot.txt",
        "puzzle_platinum.txt"
    };
    const char *names[] = {
        "Easy",
        "Medium",
        "AI Escargot",
        "Platinum Blonde"
    };
    
    FILE *output = fopen("benchmark_results.csv", "w");
    if (!output) {
        printf("Error: Cannot create output file\n");
        return 1;
    }
    
    fprintf(output, "Threads,Puzzle,Serial Time,Parallel V1,Parallel V2,Parallel V3,V1 Speedup,V2 Speedup,V3 Speedup\n");
    
    for (int t = 1; t <= 8; t *= 2) {
        for (int p = 0; p < 4; p++) {
            run_benchmark(puzzles[p], names[p], t, output);
        }
    }
    
    fclose(output);
    printf("Benchmark complete! Results saved to benchmark_results.csv\n");
    
    return 0;
}
