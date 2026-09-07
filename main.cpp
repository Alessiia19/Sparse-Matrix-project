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
    std::string matrix_dir = "matrices/"; 
    std::string matrix_name = "west";  
    std::string filename = matrix_dir + matrix_name + ".mtx";
    std::string matrix_label = matrix_name;
    std::string csv_file = "benchmark_results.csv";

    init_csv(csv_file);
    FormatCOO A_coo = load_mtx(filename);
    sort_coo_matrix(A_coo);
    //print_coo_matrix(A_coo);

    std::vector<double> x(A_coo.num_cols, 1.0);
    std::cout << "Matrix " << matrix_label << " loaded: " << A_coo.num_rows << "x" << A_coo.num_cols << ", NNZ = " << A_coo.nnz << "\n\n";

    // --- COO ---
    size_t mem_coo = get_memory_coo(A_coo);
    auto res_coo = run_benchmark(matrix_label, "COO", mem_coo, A_coo.nnz, A_coo.num_rows, A_coo.num_cols, 1.0, [&](){return spmv_coo(A_coo, x);});
    append_result_csv(csv_file, res_coo);
    std::cout << "[COO] Mem: " << res_coo.memory_megabytes << " MB | Time: " << res_coo.time_ms << " ms | GFLOPS: " << res_coo.gflops << "\n";
    std::vector<double> y_coo = spmv_coo(A_coo, x);
    //print_result(y_coo, "COO");

    // --- CSR ---
    FormatCSR A_csr = convert_coo_to_csr(A_coo);
    test_format(matrix_label, "CSR", get_memory_csr(A_csr), 1.0, A_coo, y_coo, csv_file, [&](){return spmv_csr(A_csr, x);});
    
    // --- ELLPACK ---
    FormatELL A_ell = convert_coo_to_ell(A_coo);
    double allocation_ratio_ell = static_cast<double>(A_ell.coef.size()) / A_coo.nnz;
    test_format(matrix_label, "ELLPACK", get_memory_ell(A_ell), allocation_ratio_ell, A_coo, y_coo, csv_file, [&](){return spmv_ell(A_ell, x);});

    // --- BSR ---
    int block_size = 2;
    FormatBSR A_bsr = convert_coo_to_bsr(A_coo, block_size);
    double allocation_ratio_bsr = static_cast<double>(A_bsr.values.size()) / A_coo.nnz;
    test_format(matrix_label, "BSR", get_memory_bsr(A_bsr), allocation_ratio_bsr, A_coo, y_coo, csv_file, [&](){return spmv_bsr(A_bsr, x);});
     
    // --- HYB ---
    FormatHYB A_hyb = convert_coo_to_hyb(A_coo);
    size_t mem_hyb = get_memory_ell(A_hyb.ell) + get_memory_coo(A_hyb.coo);
    double allocation_ratio_hyb = static_cast<double>(A_hyb.ell.coef.size() + A_hyb.coo.values.size()) / A_coo.nnz;
    test_format(matrix_label, "HYB", mem_hyb, allocation_ratio_hyb, A_coo, y_coo, csv_file, [&](){return spmv_hyb(A_hyb, x);});
    
    // --- BCE ---
    FormatBCE A_bce = convert_coo_to_bce(A_coo);
    size_t mem_bce = get_memory_ell(A_bce.ell) + get_memory_csr(A_bce.csr);
    double allocation_ratio_bce = static_cast<double>(A_bce.ell.coef.size() + A_bce.csr.values.size()) / A_coo.nnz;
    test_format(matrix_label, "BCE", mem_bce, allocation_ratio_bce, A_coo, y_coo, csv_file, [&](){return spmv_bce(A_bce, x);});
    
    std::cout << "\nBenchmark completed. Results saved in " << csv_file << "\n";
    return 0;
}