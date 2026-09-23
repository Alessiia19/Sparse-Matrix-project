#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "matrix_coo.hpp"
#include "matrix_bsr.hpp"
#include "metrics.hpp"
#include "utils.hpp"

int main() {
    std::string matrix_dir = "matrices/"; 
    std::vector<std::string> matrices = {"west0497", "wiki-Vote", "shipsec1", "cnr-2000", "amazon0302", "bmw3_2"};
    std::vector<int> block_sizes = {2, 4, 8};
    std::string csv_file = "bsr_benchmark_results.csv";

    init_csv(csv_file);

    for (const auto& matrix_name : matrices) {
        std::string filename = matrix_dir + matrix_name + ".mtx";
        std::cout << "Loading matrix: " << matrix_name << "...\n";
        FormatCOO A_coo = load_matrix(filename);
        sort_coo_matrix(A_coo);

        std::cout << "Matrix " << matrix_name << " loaded: "<< A_coo.num_rows << "x" << A_coo.num_cols << ", NNZ = " << A_coo.nnz << "\n";

        std::vector<double> x(A_coo.num_cols, 1.0);
        std::vector<double> y_coo = spmv_coo(A_coo, x);

        // Benchmark per block_size
        for (int b : block_sizes) {
            std::string format_label = "BSR (b=" + std::to_string(b) + ")";
            std::cout << "  Benchmarking " << format_label << "...\n";

            auto start_bsr = std::chrono::high_resolution_clock::now();
            FormatBSR A_bsr = convert_coo_to_bsr(A_coo, b);
            auto end_bsr = std::chrono::high_resolution_clock::now();
            
            double conv_time_bsr_ms = std::chrono::duration<double, std::milli>(end_bsr - start_bsr).count();
            double allocation_ratio_bsr = static_cast<double>(A_bsr.values.size()) / A_coo.nnz;

            test_format(
                matrix_name, 
                format_label, 
                get_memory_bsr(A_bsr), 
                allocation_ratio_bsr, 
                conv_time_bsr_ms, 
                A_coo, 
                y_coo, 
                csv_file, 
                [&](){ return spmv_bsr(A_bsr, x); }
            );
        }
        std::cout << "\n";
    }

    std::cout << "Benchmark completed successfully. Results saved in " << csv_file << "\n";
    return 0;
}