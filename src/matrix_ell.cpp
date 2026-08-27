#include "matrix_ell.hpp"
#include <algorithm>

FormatELL convert_coo_to_ell(const FormatCOO& coo) {
    FormatELL ell;
    ell.num_rows = coo.num_rows;
    ell.num_cols = coo.num_cols;

    // Computation of the number of non-zero elements per row and determination of the maximum
    std::vector<int> row_counts(ell.num_rows, 0);
    for (int r : coo.row_indices) {
        row_counts[r]++;
    }

    ell.max_nnz_per_row = 0;
    for (int count : row_counts) {
        ell.max_nnz_per_row = std::max(ell.max_nnz_per_row, count);
    }

    size_t matrix_size = static_cast<size_t>(ell.num_rows) * ell.max_nnz_per_row;
    ell.coef.assign(matrix_size, 0.0);
    ell.jcoef.assign(matrix_size, 0); 

    std::vector<int> current_pos(ell.num_rows, 0);

    for (int i = 0; i < coo.nnz; ++i) {
        int row = coo.row_indices[i];
        int column = coo.col_indices[i];
        double value = coo.values[i];

        int dest_idx = row * ell.max_nnz_per_row + current_pos[row];
        ell.coef[dest_idx] = value;
        ell.jcoef[dest_idx] = column;
        current_pos[row]++;
    }

    return ell;
}


// Matrix-vector product
std::vector<double> spmv_ell(const FormatELL& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);

    for (int i = 0; i < A.num_rows; ++i) {
        double sum = 0.0;
        int row_start = i * A.max_nnz_per_row;

        for (int j = 0; j < A.max_nnz_per_row; ++j) {
            int index = row_start + j;
            sum += A.coef[index] * x[A.jcoef[index]];
        }
        y[i] = sum;
    }

    return y;
}