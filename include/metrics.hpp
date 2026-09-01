#ifndef METRICS_HPP
#define METRICS_HPP

#include <string>
#include <vector>
#include <functional>
#include "matrix_coo.hpp"
#include "matrix_csr.hpp"
#include "matrix_ell.hpp"
#include "matrix_bsr.hpp"

struct BenchmarkResult {
    std::string matrix;
    std::string format;
    size_t memory_bytes = 0;
    double memory_megabytes = 0.0;
    double time_ms = 0.0;
    double gflops = 0.0;
    double bandwidth_gbs = 0.0;
    double density = 1.0;
};

// Memory usage calculation in bytes
size_t get_memory_coo(const FormatCOO& A);
size_t get_memory_csr(const FormatCSR& A);
size_t get_memory_ell(const FormatELL& A);
size_t get_memory_bsr(const MatrixBSR& A);

// SpMV benchmark execution including warm-up
BenchmarkResult run_benchmark(const std::string& matrix, 
    const std::string& format,
    size_t memory_bytes,
    int nnz,
    int num_rows,
    int num_cols,
    double density,
    std::function<std::vector<double>()> spmv_func,
    int warmup_iterations = 10,
    int test_iterations = 20
);


#endif