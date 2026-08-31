#include "metrics.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

size_t get_memory_coo(const FormatCOO& A) {
    return A.row_indices.size() * sizeof(int) +
           A.col_indices.size() * sizeof(int) +
           A.values.size() * sizeof(double);
}

size_t get_memory_csr(const FormatCSR& A) {
    return A.row_pointers.size() * sizeof(int) +
           A.col_indices.size() * sizeof(int) +
           A.values.size() * sizeof(double);
}

size_t get_memory_ell(const FormatELL& A) {
    return A.jcoef.size() * sizeof(int) +
           A.coef.size() * sizeof(double);
}

size_t get_memory_bsr(const MatrixBSR& A) {
    return A.pointerB.size() * sizeof(int) +
           A.pointerE.size() * sizeof(int) +
           A.columns.size() * sizeof(int) +
           A.values.size() * sizeof(double);
}

void init_csv(const std::string& csv_filename) {
    std::ofstream file(csv_filename, std::ios::trunc);
    if (file.is_open()) {
        file << "Matrix,Format,Memory_MB,Time_ms,GFLOPS,Bandwidth_GBs,FillFactor\n";
        file.close();
    }
}

void append_result_csv(const std::string& csv_filename, const BenchmarkResult& res) {
    std::ofstream file(csv_filename, std::ios::app);
    if (file.is_open()) {
        file << res.matrix_name << ","
             << res.format_name << ","
             << res.memory_mb << ","
             << res.time_ms << ","
             << res.gflops << ","
             << res.bandwidth_gbs << ","
             << res.fill_factor << "\n";
        file.close();
    }
}

BenchmarkResult run_benchmark(
    const std::string& matrix_name,
    const std::string& format_name,
    size_t memory_bytes,
    int nnz_reali,
    int num_rows,
    int num_cols,
    double fill_factor,
    std::function<std::vector<double>()> spmv_func,
    int warmup_iterations,
    int bench_iterations
) {
    // 1. Warm-up: Esecuzioni a vuoto per caricare le strutture in Cache
    for (int i = 0; i < warmup_iterations; ++i) {
        volatile auto dummy = spmv_func();
        (void)dummy;
    }

    // 2. Benchmarking: Misurazione precisa del tempo medio
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < bench_iterations; ++i) {
        volatile auto dummy = spmv_func();
        (void)dummy;
    }
    auto end = std::chrono::high_resolution_clock::now();

    double total_time_sec = std::chrono::duration<double>(end - start).count();
    double avg_time_sec = total_time_sec / bench_iterations;
    double avg_time_ms = avg_time_sec * 1000.0;

    // 3. Calcolo GFLOPS (2 * NNZ operazioni floating point)
    double gflops = (2.0 * nnz_reali) / (avg_time_sec * 1e9);

    // 4. Calcolo Banda Passante (Byte Letti Matrice + Vettore x + Byte Scritti y)
    size_t vector_bytes = (num_cols + num_rows) * sizeof(double);
    size_t total_bytes_transferred = memory_bytes + vector_bytes;
    double bandwidth_gbs = total_bytes_transferred / (avg_time_sec * 1e9);

    BenchmarkResult res;
    res.matrix_name = matrix_name;
    res.format_name = format_name;
    res.memory_bytes = memory_bytes;
    res.memory_mb = static_cast<double>(memory_bytes) / (1024.0 * 1024.0);
    res.time_ms = avg_time_ms;
    res.gflops = gflops;
    res.bandwidth_gbs = bandwidth_gbs;
    res.fill_factor = fill_factor;

    return res;
}