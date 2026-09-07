# Sparse-Matrix-project Overview
A comparative study of various sparse matrix storage formats and their performance impact on SpMV operations across different architectural workloads.

The project evaluates the following formats:
* **Standard Formats:** COO (Coordinate Format), CSR (Compressed Sparse Row), ELLPACK, BSR (Block Sparse Row)
* **Hybrid Formats:** HYB (ELLPACK + COO), BCE (ELLPACK + CSR)

## Test Matrices
We evaluate performance using datasets from the **SuiteSparse Matrix Collection**:
* `wiki-Vote`
* `west0497`
* `bmw3_2`

## Building and Running
To compile and execute the benchmark:

```bash
# Compilation
g++ -std=c++17 -O3 -Iinclude main.cpp src/*.cpp -o main

# Execution
./main