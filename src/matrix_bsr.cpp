#include "matrix_bsr.hpp"
#include <map>
#include <iostream>
#include <algorithm>

MatrixBSR convert_coo_to_bsr(const FormatCOO& coo, int block_size) {
    MatrixBSR bsr;
    bsr.num_rows = coo.num_rows;
    bsr.num_cols = coo.num_cols;
    bsr.block_size = block_size;

    // Calcolo del numero di blocchi (per righe e per colonne)
    // VEDI SE CAMBIARE COME SI CALCOLA BLOCK SIZE
    bsr.num_block_rows = (coo.num_rows + block_size - 1) / block_size; 
    bsr.num_block_cols = (coo.num_cols + block_size - 1) / block_size;

    /* Block_grid: array di supporto che servirà per creare gli array: 
        - values
        - columns
        - pointerB
        - pointerE 

      Ogni cella dell'array rappresenta una riga di blocchi e ciascun blocco in essa è mappato con:
      - int: indice della colonna del blocco
      - vector<double>: array contenente i valori degli elementi all'interno del blocco
     */
    std::vector<std::map<int, std::vector<double>>> block_grid(bsr.num_block_rows);

    for (int i = 0; i < coo.nnz; ++i) {
        int row = coo.row_indices[i];
        int col = coo.col_indices[i];
        double val = coo.values[i];

        // Indici (globali) di riga e colonna del blocco
        int block_row_idx = row / block_size;
        int block_col_idx = col / block_size;

        // Indici (locali) di riga e colonna all'interno del blocco
        // Compresi in un intervallo [0, block_size - 1]
        int local_row_idx = row % block_size;
        int local_col_idx = col % block_size;

        // Se il blocco non esiste già alloca lo spazio necessario inizializzandolo a 0.0
        if (block_grid[block_row_idx][block_col_idx].empty()) {
            block_grid[block_row_idx][block_col_idx].assign(block_size * block_size, 0.0);
        }

        // Indice locale dell'elemento all'interno del blocco
        int in_block_idx = local_row_idx * block_size + local_col_idx;
        block_grid[block_row_idx][block_col_idx][in_block_idx] = val;
    }

    
    // Costruzione dei 4 vettori del formato BSR 
    bsr.pointerB.resize(bsr.num_block_rows);
    bsr.pointerE.resize(bsr.num_block_rows);

    int current_block_idx = 0; 

    for (int block_row_idx = 0; block_row_idx < bsr.num_block_rows; ++block_row_idx) {
        int blocks_in_row = block_grid[block_row_idx].size();

        // Indice di partenza del primo blocco della riga corrente nel vettore columns
        bsr.pointerB[block_row_idx] = current_block_idx;
        
        // Indice successivo all'ultimo blocco della riga corrente
        bsr.pointerE[block_row_idx] = current_block_idx + blocks_in_row;

        current_block_idx += blocks_in_row;
        bsr.nnz_blocks += blocks_in_row;

        
        for (const auto& [block_col_idx, block_values] : block_grid[block_row_idx]) {
            bsr.columns.push_back(block_col_idx);
            
            for (double v : block_values) {
                bsr.values.push_back(v);
            }
        }
    }

    return bsr;
}


// Matrix-vector product
std::vector<double> spmv_bsr(const MatrixBSR& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);
    int block_size = A.block_size;

    for (int block_row_idx = 0; block_row_idx < A.num_block_rows; ++block_row_idx) {
        
        int row_start = A.pointerB[block_row_idx];
        int row_end = A.pointerE[block_row_idx];

        for (int i = row_start; i < row_end; ++i) {
            int block_col_idx = A.columns[i]; 
            
            // Ogni blocco ha dimensione b*b: per saltare al i-esimo blocco dobbiamo saltare i * b^2 valori
            int value_idx = i * block_size * block_size; 

            
            for (int local_row_idx = 0; local_row_idx < block_size; ++local_row_idx) {
                
                // Indice globale della riga nella matrice originale
                int global_row_idx = block_row_idx * block_size + local_row_idx;
                
                double sum = 0.0;
                for (int local_col_idx = 0; local_col_idx < block_size; ++local_col_idx) {
                    
                    // Indice globale della colonna nella matrice originale
                    int global_col_idx = block_col_idx * block_size + local_col_idx;
                    
                    // Indice globale del valore in values
                    int global_value_idx = value_idx + (local_row_idx * block_size + local_col_idx);
                    sum += A.values[global_value_idx] * x[global_col_idx];
                    
                }
                
                y[global_row_idx] += sum;
            }
        }
    }
    
    return y;
}