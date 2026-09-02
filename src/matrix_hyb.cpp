#include "matrix_hyb.hpp"
#include "utils.hpp"

FormatHYB convert_coo_to_hyb(const FormatCOO& coo) {
    FormatHYB hyb;
    hyb.num_rows = coo.num_rows;
    hyb.num_cols = coo.num_cols;

    int K = get_k(coo);

    // ELL creation
    hyb.ell.num_rows = coo.num_rows;
    hyb.ell.num_cols = coo.num_cols;
    hyb.ell.max_nnz_per_row = K;
    
    size_t ell_size = static_cast<size_t>(coo.num_rows) * K;
    hyb.ell.coef.assign(ell_size, 0.0);
    hyb.ell.jcoef.assign(ell_size, 0);

    // COO creation
    hyb.coo.num_rows = coo.num_rows;
    hyb.coo.num_cols = coo.num_cols;

    
    // HYB creation
    std::vector<int> current_pos(coo.num_rows, 0);
    
    for (int i = 0; i < coo.nnz; ++i) {
        int row = coo.row_indices[i];
        int col = coo.col_indices[i];
        double value = coo.values[i];

        if (current_pos[row] < K) {
            int dest_idx = row * K + current_pos[row];
            hyb.ell.coef[dest_idx] = value;
            hyb.ell.jcoef[dest_idx] = col;
            current_pos[row]++;
        } else {
            hyb.coo.row_indices.push_back(row);
            hyb.coo.col_indices.push_back(col);
            hyb.coo.values.push_back(value);
            hyb.coo.nnz++;
        }
    }

    return hyb;
}

// Matrix-vector product
std::vector<double> spmv_hyb(const FormatHYB& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);


    for (int i = 0; i < A.ell.num_rows; ++i) {
        double sum = 0.0;
        int row_start = i * A.ell.max_nnz_per_row;

        for (int j = 0; j < A.ell.max_nnz_per_row; ++j) {
            int index = row_start + j;
            sum += A.ell.coef[index] * x[A.ell.jcoef[index]];
        }
        y[i] = sum;
    }

    
    for (int i = 0; i < A.coo.nnz; ++i) {
        int row = A.coo.row_indices[i];
        int col = A.coo.col_indices[i];
        double val = A.coo.values[i];
        
        y[row] += val * x[col];
    }

    return y;
}