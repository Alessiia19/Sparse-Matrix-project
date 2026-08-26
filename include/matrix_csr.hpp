#ifndef MATRIX_CSR_HPP
#define MATRIX_CSR_HPP

#include <vector>
#include "matrix_coo.hpp"

struct MatrixCSR {
    int num_rows = 0;
    int num_cols = 0;
    int nnz = 0;

    std::vector<int> row_ptr;     // Dim: num_rows + 1
    std::vector<int> col_indices; // Dim: nnz
    std::vector<double> values;   // Dim: nnz
};


MatrixCSR convert_coo_to_csr(const FormatCOO& coo);
std::vector<double> spmv_csr(const MatrixCSR& A, const std::vector<double>& x);

#endif