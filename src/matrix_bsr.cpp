#include "matrix_bsr.hpp"
#include <map>

MatrixBSR convert_coo_to_bsr(const FormatCOO& coo, int block_size) {
    MatrixBSR bsr;
    bsr.num_rows = coo.num_rows;
    bsr.num_cols = coo.num_cols;
    bsr.block_size = block_size;

    bsr.num_block_rows = (coo.num_rows + block_size - 1) / block_size;
    bsr.num_block_cols = (coo.num_cols + block_size - 1) / block_size;

    // Struttura temporanea per raggruppare gli elementi in blocchi densi
    std::vector<std::map<int, std::vector<double>>> block_rows(bsr.num_block_rows);

    for (int i = 0; i < coo.nnz; ++i) {
        int r = coo.row_indices[i];
        int c = coo.col_indices[i];
        double v = coo.values[i];

        int br = r / block_size;
        int bc = c / block_size;

        int local_r = r % block_size;
        int local_c = c % block_size;

        if (block_rows[br].find(bc) == block_rows[br].end()) {
            block_rows[br][bc].assign(block_size * block_size, 0.0);
        }

        int local_idx = local_r * block_size + local_c;
        block_rows[br][bc][local_idx] = v;
    }

    // Ridimensioniamo i vettori pointerB e pointerE in base al numero di righe di blocchi
    bsr.pointerB.resize(bsr.num_block_rows);
    bsr.pointerE.resize(bsr.num_block_rows);

    int current_block_idx = 0;

    for (int br = 0; br < bsr.num_block_rows; ++br) {
        int blocks_in_row = block_rows[br].size();

        // pointerB segna l'indice di partenza del primo blocco di questa riga
        bsr.pointerB[br] = current_block_idx;
        
        // pointerE segna l'indice immediatamente successivo all'ultimo blocco di questa riga
        bsr.pointerE[br] = current_block_idx + blocks_in_row;

        // Avanziamo l'indice globale per la riga successiva
        current_block_idx += blocks_in_row;
        bsr.nnz_blocks += blocks_in_row;

        // Inseriamo gli indici delle colonne e i valori dei blocchi
        for (const auto& [bc, block_values] : block_rows[br]) {
            bsr.b_col_indices.push_back(bc);
            bsr.b_values.insert(bsr.b_values.end(), block_values.begin(), block_values.end());
        }
    }

    return bsr;
}

std::vector<double> spmv_bsr(const MatrixBSR& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);
    int b = A.block_size;

    for (int br = 0; br < A.num_block_rows; ++br) {
        // Usiamo pointerB per l'inizio e pointerE per il limite superiore
        int block_row_start = A.pointerB[br];
        int block_row_end = A.pointerE[br];

        for (int j = block_row_start; j < block_row_end; ++j) {
            int bc = A.b_col_indices[j];
            int val_offset = j * b * b;

            for (int local_r = 0; local_r < b; ++local_r) {
                int global_r = br * b + local_r;
                if (global_r >= A.num_rows) continue;

                double sum = 0.0;
                for (int local_c = 0; local_c < b; ++local_c) {
                    int global_c = bc * b + local_c;
                    
                    if (global_c < A.num_cols) {
                        int local_idx = val_offset + (local_r * b + local_c);
                        sum += A.b_values[local_idx] * x[global_c];
                    }
                }
                y[global_r] += sum;
            }
        }
    }

    return y;
}