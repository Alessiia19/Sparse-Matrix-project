#ifndef MATRIX_COO_HPP
#define MATRIX_COO_HPP

#include <vector>
#include <string>

struct FormatCOO {
    int num_rows = 0;
    int num_cols = 0;
    int nnz = 0;
    
    std::vector<int> row_indices;
    std::vector<int> col_indices;
    std::vector<double> values;
};


FormatCOO load_matrix(const std::string& filename);
std::vector<double> spmv_coo(const FormatCOO& A, const std::vector<double>& x);

#endif