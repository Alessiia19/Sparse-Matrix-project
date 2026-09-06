#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include "matrix_coo.hpp"

struct BenchmarkResult;

// CSV export
void init_csv(const std::string& csv_filename);
void append_result_csv(const std::string& csv_filename, const BenchmarkResult& res);

// Calculate average nnz per row (for hybrid formats)
int get_k(const FormatCOO& coo);

// Sort COO matrix in row-major order
void sort_coo_matrix(FormatCOO& coo);

void print_coo_matrix(const FormatCOO& coo, int max_elements = -1);

// Sanity check function to validate SpMV output with COO ground truth
void sanity_check(const std::vector<double>& y_ref, const std::vector<double>& y_test, const std::string& format_name, double tolerance);

// Print result of spmv
void print_result(const std::vector<double>& y, const std::string& format_name);

#endif 