#ifndef MATRIX_BSR_HPP
#define MATRIX_BSR_HPP

#include <vector>
#include "matrix_coo.hpp"

struct MatrixBSR {
    int num_rows = 0;
    int num_cols = 0;
    int block_size = 1;       // Dimensione b del blocco (es. 2 per 2x2)
    int num_block_rows = 0;   // Numero di righe di blocchi
    int num_block_cols = 0;   // Numero di colonne di blocchi
    int nnz_blocks = 0;       // Numero totale di blocchi non nulli

    std::vector<int> pointerB;       // Dimensione: num_block_rows (Inizio blocchi per riga)
    std::vector<int> pointerE;       // Dimensione: num_block_rows (Fine blocchi + 1 per riga)
    std::vector<int> b_col_indices; // Indici di colonna dei blocchi
    std::vector<double> b_values;   // Valori raggruppati in sottomatrici b x b
};

MatrixBSR convert_coo_to_bsr(const FormatCOO& coo, int block_size);
std::vector<double> spmv_bsr(const MatrixBSR& A, const std::vector<double>& x);

#endif