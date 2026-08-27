#include "matrix_csr.hpp"

FormatCSR convert_coo_to_csr(const FormatCOO& coo) {
    FormatCSR csr;
    csr.num_rows = coo.num_rows;
    csr.num_cols = coo.num_cols;
    csr.nnz = coo.nnz;

    csr.values = coo.values;
    csr.col_indices = coo.col_indices;
    
    
    // Count of non-zero elements in each row
    std::vector<int> row_counts(csr.num_rows, 0);
    for (int row : coo.row_indices) {
        row_counts[row]++;
    }
    
    csr.row_pointers.resize(csr.num_rows + 1);
    csr.row_pointers[0] = 0;
    
    // Computation of row-start pointers
    for (int i = 0; i < csr.num_rows; ++i) {
        csr.row_pointers[i + 1] = csr.row_pointers[i] + row_counts[i];
    }

    return csr;
}

// Matrix-vector product
std::vector<double> spmv_csr(const FormatCSR& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);

    for (int i = 0; i < A.num_rows; ++i) {
        double sum = 0.0;
        int row_start = A.row_pointers[i];
        int row_end = A.row_pointers[i + 1];

        for (int j = row_start; j < row_end; ++j) {
            sum += A.values[j] * x[A.col_indices[j]];
        }
        y[i] = sum;
    }

    return y;
}