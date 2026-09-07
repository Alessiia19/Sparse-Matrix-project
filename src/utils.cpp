#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm> 
#include <numeric>
#include "metrics.hpp"

void init_csv(const std::string& csv_filename) {
    std::ofstream file(csv_filename, std::ios::trunc);
    if (file.is_open()) {
        file << "Matrix,Format,Memory_MB,Time_ms,GFLOPS,Bandwidth_GBs,Allocation_Ratio\n";
        file.close();
    }
}

void append_result_csv(const std::string& csv_filename, const BenchmarkResult& res) {
    std::ofstream file(csv_filename, std::ios::app);
    if (file.is_open()) {
        file << res.matrix << ","
             << res.format << ","
             << res.memory_megabytes << ","
             << res.time_ms << ","
             << res.gflops << ","
             << res.bandwidth_gbs << ","
             << res.allocation_ratio << "\n";
        file.close();
    }
}

int get_k(const FormatCOO& coo) {
    double average_nnz_per_row = static_cast<double>(coo.nnz) / coo.num_rows;
    return static_cast<int>(std::ceil(average_nnz_per_row));
}

void sort_coo_matrix(FormatCOO& coo) {

    std::vector<size_t> sort_order(coo.nnz);
    for (size_t i = 0; i < sort_order.size(); ++i) {
        sort_order[i] = i;
    }

    std::stable_sort(sort_order.begin(), sort_order.end(), [&](size_t i, size_t j) {
        if (coo.row_indices[i] != coo.row_indices[j])
            return coo.row_indices[i] < coo.row_indices[j];
        return coo.col_indices[i] < coo.col_indices[j];
    });

    std::vector<int> sorted_rows(coo.nnz);
    std::vector<int> sorted_cols(coo.nnz);
    std::vector<double> sorted_vals(coo.nnz);

    for (size_t i = 0; i < static_cast<size_t>(coo.nnz); ++i) {
        sorted_rows[i] = coo.row_indices[sort_order[i]];
        sorted_cols[i] = coo.col_indices[sort_order[i]];
        sorted_vals[i] = coo.values[sort_order[i]];
    }

    coo.row_indices = std::move(sorted_rows);
    coo.col_indices = std::move(sorted_cols);
    coo.values = std::move(sorted_vals);
}

void print_coo_matrix(const FormatCOO& coo, int max_elements) {
    int limit = (max_elements < 0 || max_elements > coo.nnz) ? coo.nnz : max_elements;

    std::cout << "--- Print Matrix COO (" << limit << " of " << coo.nnz << " elements) ---" << std::endl;
    for (int i = 0; i < limit; ++i) {
        std::cout << "Element " << i << " -> Row: " << coo.row_indices[i]
                  << " | Col: " << coo.col_indices[i]
                  << " | Val: " << coo.values[i] << std::endl;
    }
    std::cout << "--------------------------------------------------------\n" << std::endl;
}

void print_result(const std::vector<double>& y, const std::string& format_name) {
    std::cout << "--- Result y = A * x (Format " << format_name << ") ---\n";
    for (size_t i = 0; i < y.size(); ++i) {
        std::cout << "y[" << i << "] = " << y[i] << "\n";
    }
    std::cout << "\n";
}

void sanity_check(const std::vector<double>& y_ref, const std::vector<double>& y_test, const std::string& format_name, double tol) {
    if (y_ref.size() != y_test.size()) {
        std::cerr << "[" << format_name << " SANITY CHECK FAILED] Size mismatch: "
                  << "expected " << y_ref.size() << ", got " << y_test.size() << std::endl;
        return;
    }

    int error_count = 0;
    const int max_errors_to_print = 5;

    for (size_t i = 0; i < y_ref.size(); ++i) {
        double abs_diff = std::abs(y_ref[i] - y_test[i]);
        double abs_ref = std::abs(y_ref[i]);

        double err = (abs_ref > 1.0) ? (abs_diff / abs_ref) : abs_diff;

        if (err > tol) {
            if (error_count == 0) {
                std::cout << "[" << format_name << " SANITY CHECK FAILED]\n";
            }

            if (error_count < max_errors_to_print) {
                std::cout << "  -> Index " << i 
                          << " | Ref: " << y_ref[i] 
                          << " | Test: " << y_test[i] 
                          << " | Abs Diff: " << abs_diff 
                          << " | Rel Err: " << (abs_diff / (abs_ref > 0 ? abs_ref : 1.0)) << "\n";
            }
            error_count++;
        }
    }

    if (error_count == 0) {
        std::cout << "[" << format_name << " SANITY CHECK PASSED]\n";
    }
}