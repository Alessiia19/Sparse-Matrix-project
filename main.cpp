#include <iostream>
#include <vector>
#include <string>
#include "matrix_coo.hpp"
#include "matrix_csr.hpp"
#include "matrix_ell.hpp"
#include "matrix_bsr.hpp"
#include "matrix_hyb.hpp"
#include "matrix_bce.hpp"
#include "metrics.hpp"
#include "utils.hpp"

int main() {
    std::string filename = "test2.mtx";
    std::string matrix_label = "test2";
    std::string csv_file = "benchmark_results.csv";

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
        std::cout << "[COO] Mem: " << res.memory_megabytes << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";

        std::vector<double> y_coo = spmv_coo(A_coo, x);
        std::cout << "--- Result y = A * x (Format COO) ---" << std::endl;
        for (size_t i = 0; i < y_coo.size(); ++i) {
            std::cout << "y[" << i << "] = " << y_coo[i] << std::endl;
        }
    }
    
    

    // --- 2. CSR BENCHMARK ---
    {
        FormatCSR A_csr = convert_coo_to_csr(A_coo);
        size_t mem = get_memory_csr(A_csr);
        auto res = run_benchmark(matrix_label, "CSR", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, 1.0,
            [&]() { return spmv_csr(A_csr, x); });
        append_result_csv(csv_file, res);
        std::cout << "[CSR] Mem: " << res.memory_megabytes << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
        
        std::vector<double> y_csr = spmv_csr(A_csr, x);
        std::cout << "--- Result y = A * x (Format CSR) ---" << std::endl;
        for (size_t i = 0; i < y_csr.size(); ++i) {
            std::cout << "y[" << i << "] = " << y_csr[i] << std::endl;
        }
    }


    // --- 3. ELLPACK BENCHMARK ---
    {
        FormatELL A_ell = convert_coo_to_ell(A_coo);
        size_t mem = get_memory_ell(A_ell);
        double allocation_ratio = static_cast<double>(A_ell.coef.size()) / A_coo.nnz;
        auto res = run_benchmark(matrix_label, "ELLPACK", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, allocation_ratio,
            [&]() { return spmv_ell(A_ell, x); });
        append_result_csv(csv_file, res);
        std::cout << "[ELL] Mem: " << res.memory_megabytes << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
        
        std::vector<double> y_ell = spmv_ell(A_ell, x);
        std::cout << "--- Result y = A * x (Format ELL) ---" << std::endl;
        for (size_t i = 0; i < y_ell.size(); ++i) {
            std::cout << "y[" << i << "] = " << y_ell[i] << std::endl;
        }
    }


    // --- 4. BSR BENCHMARK ---
    {
        int block_size = 3;
        FormatBSR A_bsr = convert_coo_to_bsr(A_coo, block_size);
        size_t mem = get_memory_bsr(A_bsr);
        double allocation_ratio = static_cast<double>(A_bsr.values.size()) / A_coo.nnz;
        auto res = run_benchmark(matrix_label, "BSR", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, allocation_ratio,
            [&]() { return spmv_bsr(A_bsr, x); });
        append_result_csv(csv_file, res);
        std::cout << "[BSR] Mem: " << res.memory_megabytes << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
        
        std::vector<double> y_bsr = spmv_bsr(A_bsr, x);
        std::cout << "--- Result y = A * x (Format BSR) ---" << std::endl;
        for (size_t i = 0; i < y_bsr.size(); ++i) {
            std::cout << "y[" << i << "] = " << y_bsr[i] << std::endl;
        }
    }

    // --- HYB BENCHMARK ---
    {
        
        FormatHYB A_hyb = convert_coo_to_hyb(A_coo);
        size_t mem = get_memory_ell(A_hyb.ell) + get_memory_coo(A_hyb.coo);
        
        size_t total_elements_allocated = A_hyb.ell.coef.size() + A_hyb.coo.values.size();
        double allocation_ratio = static_cast<double>(total_elements_allocated) / A_coo.nnz;
        
        auto res = run_benchmark(matrix_label, "HYB", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, allocation_ratio,
            [&]() { return spmv_hyb(A_hyb, x); });
        append_result_csv(csv_file, res);
        
        std::cout << "[HYB] Mem: " << res.memory_megabytes << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
        
        std::vector<double> y_hyb = spmv_hyb(A_hyb, x);
        std::cout << "--- Result y = A * x (Format HYB) ---" << std::endl;
        for (size_t i = 0; i < y_hyb.size(); ++i) {
            std::cout << "y[" << i << "] = " << y_hyb[i] << std::endl;
        }
    }

    // --- BCE BENCHMARK ---
    {
        FormatBCE A_bce = convert_coo_to_bce(A_coo);
        size_t mem = get_memory_ell(A_bce.ell) + get_memory_csr(A_bce.csr);
        
        size_t total_elements_allocated = A_bce.ell.coef.size() + A_bce.csr.values.size();
        double allocation_ratio = static_cast<double>(total_elements_allocated) / A_coo.nnz;
        
        auto res = run_benchmark(matrix_label, "BCE", mem, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, allocation_ratio,
            [&]() { return spmv_bce(A_bce, x); });
        append_result_csv(csv_file, res);
        
        std::cout << "[BCE] Mem: " << res.memory_megabytes << " MB | Time: " << res.time_ms << " ms | GFLOPS: " << res.gflops << "\n";
        
        std::vector<double> y_bce = spmv_bce(A_bce, x);
        std::cout << "--- Result y = A * x (Format BCE) ---" << std::endl;
        for (size_t i = 0; i < y_bce.size(); ++i) {
            std::cout << "y[" << i << "] = " << y_bce[i] << std::endl;
        }
    }


    std::cout << "\nBenchmark completato. Dati salvati in " << csv_file << "\n";
    return 0;
}