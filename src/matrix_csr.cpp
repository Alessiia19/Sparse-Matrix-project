#include "matrix_csr.hpp"

MatrixCSR convert_coo_to_csr(const FormatCOO& coo) {
    MatrixCSR csr;
    csr.num_rows = coo.num_rows;
    csr.num_cols = coo.num_cols;
    csr.nnz = coo.nnz;

    csr.values = coo.values;
    csr.col_indices = coo.col_indices;
    csr.row_ptr.assign(csr.num_rows + 1, 0); 
    // assegna nnz + 1 posizioni nulle nel vettore row pointer, verranno modificate sotto

    // 1. Conteggio del numero di elementi non-zero per ogni riga
    for (int r : coo.row_indices) {
        csr.row_ptr[r + 1]++;
    }

    // 2. Somma prefissa (Prefix Sum) per calcolare i puntatori di riga
    for (int i = 0; i < csr.num_rows; ++i) {
        csr.row_ptr[i + 1] += csr.row_ptr[i];
    }

    return csr;
}

std::vector<double> spmv_csr(const MatrixCSR& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);

    // Ogni iterazione del ciclo esterno gestisce una riga indipendente
    for (int i = 0; i < A.num_rows; ++i) {
        double sum = 0.0;
        int row_start = A.row_ptr[i];
        int row_end = A.row_ptr[i + 1];

        for (int j = row_start; j < row_end; ++j) {
            sum += A.values[j] * x[A.col_indices[j]];
        }
        y[i] = sum;
    }

    return y;
}