#!/usr/bin/env python3
import re
from collections import defaultdict

filename = 'results/test_results.txt'
with open(filename, 'r', encoding='utf-8') as f:
    content = f.read()

# Parse by thread count sections
thread_sections = re.split(r'=== (\d+) THREADS? ===', content)
print(f"Total sections found: {len(thread_sections)}")
print(f"Expected: should be odd number (2*num_threads + 1)")

results = defaultdict(lambda: defaultdict(dict))

for i in range(1, len(thread_sections), 2):
    threads = int(thread_sections[i])
    section = thread_sections[i + 1]
    print(f"\n=== Processing {threads} threads section ===")
    
    # Find puzzle sections
    puzzles = {
        'Easy': re.search(r'Easy Puzzle:(.*?)(?=Medium Puzzle:|AI Escargot:|Platinum Blonde:|$)', section, re.DOTALL),
        'Medium': re.search(r'Medium Puzzle:(.*?)(?=Easy Puzzle:|AI Escargot:|Platinum Blonde:|$)', section, re.DOTALL),
        'AI Escargot': re.search(r'AI Escargot:(.*?)(?=Easy Puzzle:|Medium Puzzle:|Platinum Blonde:|$)', section, re.DOTALL),
        'Platinum': re.search(r'Platinum Blonde:(.*?)(?=Easy Puzzle:|Medium Puzzle:|AI Escargot:|$)', section, re.DOTALL)
    }
    
    for puzzle_name, match in puzzles.items():
        if match:
            print(f"  Found {puzzle_name}")
            puzzle_data = match.group(1)
            print(f"    Puzzle data length: {len(puzzle_data)} chars")
            
            # Check if SERIAL VERSION is in the data
            if "=== SERIAL VERSION ===" in puzzle_data:
                print(f"    Contains SERIAL VERSION section")
            else:
                print(f"    MISSING SERIAL VERSION section")
            
            # Extract timing data  
            serial_time = re.search(r'=== SERIAL VERSION ===.*?Time taken:\s+([\d.]+)\s+seconds', puzzle_data, re.DOTALL)
            v1_speedup = re.search(r'=== PARALLEL V1.*?Speedup:\s+([\d.]+|-?nan\(ind\))x', puzzle_data, re.DOTALL)
            
            if serial_time:
                print(f"    Serial time: {serial_time.group(1)}")
                results[threads][puzzle_name]['serial_time'] = float(serial_time.group(1))
            else:
                print(f"    Serial time: NOT FOUND")
            
            if v1_speedup:
                print(f"    V1 speedup: {v1_speedup.group(1)}")

print(f"\n\nFinal results dictionary has {len(results)} thread counts")
for threads in sorted(results.keys()):
    print(f"  {threads} threads: {len(results[threads])} puzzles")
