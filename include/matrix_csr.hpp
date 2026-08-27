#ifndef MATRIX_CSR_HPP
#define MATRIX_CSR_HPP

#include <vector>
#include "matrix_coo.hpp"

struct FormatCSR {
    int num_rows = 0;
    int num_cols = 0;
    int nnz = 0;

    std::vector<int> row_pointers;     
    std::vector<int> col_indices; 
    std::vector<double> values;   
};


FormatCSR convert_coo_to_csr(const FormatCOO& coo);
std::vector<double> spmv_csr(const FormatCSR& A, const std::vector<double>& x);

#endif