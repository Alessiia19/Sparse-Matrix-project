#include "metrics.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

size_t get_memory_coo(const FormatCOO& A) {
    return A.row_indices.size() * sizeof(int) + A.col_indices.size() * sizeof(int) + A.values.size() * sizeof(double);
}

size_t get_memory_csr(const FormatCSR& A) {
    return A.row_pointers.size() * sizeof(int) + A.col_indices.size() * sizeof(int) + A.values.size() * sizeof(double);
}

size_t get_memory_ell(const FormatELL& A) {
    return A.jcoef.size() * sizeof(int) + A.coef.size() * sizeof(double);
}

size_t get_memory_bsr(const FormatBSR& A) {
    return A.pointerB.size() * sizeof(int) + A.pointerE.size() * sizeof(int) + A.columns.size() * sizeof(int) + A.values.size() * sizeof(double);
}

size_t get_memory_hyb(const FormatHYB& A) {
    return get_memory_ell(A.ell) + get_memory_coo(A.coo);
}


BenchmarkResult run_benchmark(
    const std::string& matrix,
    const std::string& format,
    size_t memory_bytes,
    int nnz,
    int num_rows,
    int num_cols,
    double allocation_ratio,
    double conversion_time_ms,
    std::function<std::vector<double>()> spmv_func,
    int warmup_iterations,
    int test_iterations
) {
    // Cache warmup
    for (int i = 0; i < warmup_iterations; ++i) {
        volatile auto x = spmv_func();
    }

    // Average time calculation
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < test_iterations; ++i) {
        volatile auto x = spmv_func();
    }
    auto end = std::chrono::high_resolution_clock::now();

    double total_time_sec = std::chrono::duration<double>(end - start).count();
    double avg_time_sec = total_time_sec / test_iterations;
    double avg_time_ms = avg_time_sec * 1000.0;

    // GFLOPS calculation
    double gflops = (2.0 * nnz) / (avg_time_sec * 1e9);

    // Memory bandwidth usage calculation 
    size_t vectors_bytes = (num_cols + num_rows) * sizeof(double);
    size_t total_bytes_transferred = memory_bytes + vectors_bytes;
    double bandwidth_gbs = total_bytes_transferred / (avg_time_sec * 1e9);

    BenchmarkResult res;
    res.matrix = matrix;
    res.format = format;
    res.memory_bytes = memory_bytes;
    res.memory_megabytes = static_cast<double>(memory_bytes) / (1024.0 * 1024.0);
    res.conversion_time_ms = conversion_time_ms;
    res.time_ms = avg_time_ms;
    res.gflops = gflops;
    res.bandwidth_gbs = bandwidth_gbs;
    res.allocation_ratio = allocation_ratio;

    return res;
}