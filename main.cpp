#include <iostream>
#include <vector>
#include <string>
#include "matrix_coo.hpp"
#include "matrix_csr.hpp"
#include "matrix_ell.hpp"
#include "matrix_bsr.hpp"
#include "metrics.hpp"

int main() {
    std::string filename = "test2.mtx";
    std::string matrix_label = "test2";
    std::string csv_file = "benchmark_results.csv";

    // Inizializza il file CSV con le intestazioni
    init_csv(csv_file);

    // Caricamento Matrice Baseline (COO)
    FormatCOO A_coo = load_mtx(filename);
    std::vector<double> x(A_coo.num_cols, 1.0);

    std::cout << "Matrice " << matrix_label << " caricata: "
              << A_coo.num_rows << "x" << A_coo.num_cols 
              << ", NNZ = " << A_coo.nnz << "\n\n";

    // --- 1. COO BENCHMARK ---
    {
        size_t mem = get_memory_coo(A_coo);
        auto res = run_benchmark(matrix_label, "COO", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, 1.0,
            [&]() { return spmv_coo(A_coo, x); });
        append_result_csv(csv_file, res);
        std::cout << "[COO] Mem: " << res.memory_mb << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
    }

    // --- 2. CSR BENCHMARK ---
    {
        FormatCSR A_csr = convert_coo_to_csr(A_coo);
        size_t mem = get_memory_csr(A_csr);
        auto res = run_benchmark(matrix_label, "CSR", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, 1.0,
            [&]() { return spmv_csr(A_csr, x); });
        append_result_csv(csv_file, res);
        std::cout << "[CSR] Mem: " << res.memory_mb << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
    }

    // --- 3. ELLPACK BENCHMARK ---
    {
        FormatELL A_ell = convert_coo_to_ell(A_coo);
        size_t mem = get_memory_ell(A_ell);
        double fill_factor = static_cast<double>(A_ell.coef.size()) / A_coo.nnz;
        auto res = run_benchmark(matrix_label, "ELLPACK", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, fill_factor,
            [&]() { return spmv_ell(A_ell, x); });
        append_result_csv(csv_file, res);
        std::cout << "[ELL] Mem: " << res.memory_mb << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
    }

    // --- 4. BSR BENCHMARK ---
    {
        int block_size = 3;
        MatrixBSR A_bsr = convert_coo_to_bsr(A_coo, block_size);
        size_t mem = get_memory_bsr(A_bsr);
        double fill_factor = static_cast<double>(A_bsr.values.size()) / A_coo.nnz;
        auto res = run_benchmark(matrix_label, "BSR", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, fill_factor,
            [&]() { return spmv_bsr(A_bsr, x); });
        append_result_csv(csv_file, res);
        std::cout << "[BSR] Mem: " << res.memory_mb << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
    }

    std::cout << "\nBenchmark completato. Dati salvati in " << csv_file << "\n";
    return 0;
}