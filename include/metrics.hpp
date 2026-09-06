#ifndef METRICS_HPP
#define METRICS_HPP

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include "matrix_coo.hpp"
#include "matrix_csr.hpp"
#include "matrix_ell.hpp"
#include "matrix_bsr.hpp"
#include "matrix_hyb.hpp"
#include "matrix_bce.hpp"
#include "utils.hpp"

struct BenchmarkResult {
    std::string matrix;
    std::string format;
    size_t memory_bytes = 0;
    double memory_megabytes = 0.0;
    double time_ms = 0.0;
    double gflops = 0.0;
    double bandwidth_gbs = 0.0;
    double allocation_ratio = 1.0;
};

// Memory usage calculation in bytes
size_t get_memory_coo(const FormatCOO& A);
size_t get_memory_csr(const FormatCSR& A);
size_t get_memory_ell(const FormatELL& A);
size_t get_memory_bsr(const FormatBSR& A);
size_t get_memory_hyb(const FormatHYB& A);

// SpMV benchmark execution including warm-up
BenchmarkResult run_benchmark(const std::string& matrix, 
    const std::string& format,
    size_t memory_bytes,
    int nnz,
    int num_rows,
    int num_cols,
    double allocation_ratio,
    std::function<std::vector<double>()> spmv_func,
    int warmup_iterations = 50,
    int test_iterations = 500
);

// Benchmark automation for each format, including sanity check of spmv results
template <typename SpMVFunc>
void test_format(
    const std::string& matrix_label,
    const std::string& format_name,
    size_t memory_bytes,
    double allocation_ratio,
    const FormatCOO& A_coo,
    const std::vector<double>& y_ref,
    const std::string& csv_file,
    SpMVFunc spmv_fn,
    double tol = 1e-9) 
{
    auto res = run_benchmark(matrix_label, format_name, memory_bytes, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, allocation_ratio, spmv_fn);

    append_result_csv(csv_file, res);
    std::cout << "[" << format_name << "] Mem: " << res.memory_megabytes << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";

    std::vector<double> y_test = spmv_fn();
    sanity_check(y_ref, y_test, format_name, tol);
}

#endif
