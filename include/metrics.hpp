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
    std::string matrix_name;
    std::string format_name;
    size_t memory_bytes = 0;
    double memory_mb = 0.0;
    double time_ms = 0.0;
    double gflops = 0.0;
    double bandwidth_gbs = 0.0;
    double fill_factor = 1.0;
};

// Calcolo memoria teorica occupata in Byte
size_t get_memory_coo(const FormatCOO& A);
size_t get_memory_csr(const FormatCSR& A);
size_t get_memory_ell(const FormatELL& A);
size_t get_memory_bsr(const MatrixBSR& A);

// Funzione generica per eseguire il benchmarking SpMV con Warm-up
BenchmarkResult run_benchmark(
    const std::string& matrix_name,
    const std::string& format_name,
    size_t memory_bytes,
    int nnz_reali,
    int num_rows,
    int num_cols,
    double fill_factor,
    std::function<std::vector<double>()> spmv_func,
    int warmup_iterations = 10,
    int bench_iterations = 20
);

// Salvataggio su CSV
void init_csv(const std::string& csv_filename);
void append_result_csv(const std::string& csv_filename, const BenchmarkResult& res);

#endif