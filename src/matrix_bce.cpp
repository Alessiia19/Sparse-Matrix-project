#include "matrix_bce.hpp"
#include "utils.hpp"
#include <cmath>
#include <algorithm>

FormatBCE convert_coo_to_bce(const FormatCOO& coo) {
    FormatBCE bce;
    bce.num_rows = coo.num_rows;
    bce.num_cols = coo.num_cols;

    int K = get_k(coo);

    // ELL creation
    bce.ell.num_rows = coo.num_rows;
    bce.ell.num_cols = coo.num_cols;
    bce.ell.max_nnz_per_row = K;
    
    size_t ell_size = static_cast<size_t>(coo.num_rows) * K;
    bce.ell.coef.assign(ell_size, 0.0);
    bce.ell.jcoef.assign(ell_size, 0);

    // --- CSR creation ---
    bce.csr.num_rows = coo.num_rows;
    bce.csr.num_cols = coo.num_cols;

    std::vector<int> current_pos(coo.num_rows, 0);
    std::vector<int> csr_row_counts(coo.num_rows, 0);

    // Count of non-zero elements in each row for CSR portion
    for (int i = 0; i < coo.nnz; ++i) {
        int row = coo.row_indices[i];
        current_pos[row]++;
        if (current_pos[row] > K) {
            csr_row_counts[row]++; 
        }
    }

    bce.csr.row_pointers.resize(bce.csr.num_rows + 1);
    bce.csr.row_pointers[0] = 0;

    // Computation of row-start pointers
    for (int i = 0; i < bce.csr.num_rows; ++i) {
        bce.csr.row_pointers[i + 1] = bce.csr.row_pointers[i] + csr_row_counts[i];
    }

    int csr_nnz = bce.csr.row_pointers[bce.csr.num_rows];
    bce.csr.nnz = csr_nnz;
    bce.csr.values.resize(csr_nnz);
    bce.csr.col_indices.resize(csr_nnz);

    
    // BCE creation
    current_pos.assign(coo.num_rows, 0);
    std::vector<int> csr_current_pos = bce.csr.row_pointers;   

    for (int i = 0; i < coo.nnz; ++i) {
        int row = coo.row_indices[i];
        int col = coo.col_indices[i];
        double value = coo.values[i];

        if (current_pos[row] < K) {
            int dest_idx = row * K + current_pos[row];
            bce.ell.coef[dest_idx] = value;
            bce.ell.jcoef[dest_idx] = col;
            current_pos[row]++;
        } else {
            int dest_idx = csr_current_pos[row];
            bce.csr.values[dest_idx] = value;
            bce.csr.col_indices[dest_idx] = col;
            csr_current_pos[row]++;
        }
    }

    return bce;
}

std::vector<double> spmv_bce(const FormatBCE& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);

    for (int i = 0; i < A.num_rows; ++i) {
        double sum = 0.0;
        
        int ell_start = i * A.ell.max_nnz_per_row;
        for (int j = 0; j < A.ell.max_nnz_per_row; ++j) {
            int index = ell_start + j;
            sum += A.ell.coef[index] * x[A.ell.jcoef[index]];
        }
        
        int csr_start = A.csr.row_pointers[i];
        int csr_end = A.csr.row_pointers[i + 1];
        for (int j = csr_start; j < csr_end; ++j) {
            sum += A.csr.values[j] * x[A.csr.col_indices[j]];
        }
        
        y[i] = sum;
    }

    return y;
}