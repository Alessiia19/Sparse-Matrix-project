#ifndef MATRIX_ELL_HPP
#define MATRIX_ELL_HPP

#include <vector>
#include "matrix_coo.hpp"

struct FormatELL
 {
    int num_rows = 0;
    int num_cols = 0;
    int max_nnz_per_row = 0;

    std::vector<int> jcoef; // Dim: num_rows * max_nnz_per_row
    std::vector<double> coef;   // Dim: num_rows * max_nnz_per_row
};

FormatELL convert_coo_to_ell(const FormatCOO& coo);
std::vector<double> spmv_ell(const FormatELL& A, const std::vector<double>& x);

#endif