#include "matrix_bsr.hpp"
#include <map>
#include <iostream>
#include <algorithm>

FormatBSR convert_coo_to_bsr(const FormatCOO& coo, int block_size) {
    FormatBSR bsr;
    bsr.num_rows = coo.num_rows;
    bsr.num_cols = coo.num_cols;
    bsr.block_size = block_size;

    // Calculation of the number of blocks (for rows and columns)
    // VEDI SE CAMBIARE COME SI CALCOLA BLOCK SIZE
    bsr.num_block_rows = coo.num_rows / block_size; 
    bsr.num_block_cols = coo.num_cols / block_size;

    /* Block_grid: helper array used to construct the following vectors: 
        - values
        - columns
        - pointerB
        - pointerE 

       Each cell of the array represents a block row, and each block within it is mapped with:
       - int: column index of the block
       - vector<double>: array containing the element values within the block
     */
    std::vector<std::map<int, std::vector<double>>> block_grid(bsr.num_block_rows);

    for (int i = 0; i < coo.nnz; ++i) {
        int row = coo.row_indices[i];
        int col = coo.col_indices[i];
        double val = coo.values[i];

        // Global row and column indices of the block
        int block_row_idx = row / block_size;
        int block_col_idx = col / block_size;

        // Local row and column indices within the block
        // Ranging within [0, block_size - 1]
        int local_row_idx = row % block_size;
        int local_col_idx = col % block_size;

        // If the block does not exist yet, allocate the necessary space initialized to 0.0
        if (block_grid[block_row_idx][block_col_idx].empty()) {
            block_grid[block_row_idx][block_col_idx].assign(block_size * block_size, 0.0);
        }

        // Local index of the element within the block
        int in_block_idx = local_row_idx * block_size + local_col_idx;
        block_grid[block_row_idx][block_col_idx][in_block_idx] = val;
    }

    
    // Construction of the 4 vectors of the BSR format
    bsr.pointerB.resize(bsr.num_block_rows);
    bsr.pointerE.resize(bsr.num_block_rows);

    int current_block_idx = 0; 

    for (int block_row_idx = 0; block_row_idx < bsr.num_block_rows; ++block_row_idx) {
        int blocks_in_row = block_grid[block_row_idx].size();

        // Starting index of the first block of the current row in the columns vector
        bsr.pointerB[block_row_idx] = current_block_idx;
        
        // Index immediately following the last block of the current row
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
std::vector<double> spmv_bsr(const FormatBSR& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);
    int block_size = A.block_size;

    for (int block_row_idx = 0; block_row_idx < A.num_block_rows; ++block_row_idx) {
        
        int row_start = A.pointerB[block_row_idx];
        int row_end = A.pointerE[block_row_idx];

        for (int i = row_start; i < row_end; ++i) {
            int block_col_idx = A.columns[i]; 
            
            // Each block has size b*b: to jump to the i-th block we must skip i * b^2 values
            int value_idx = i * block_size * block_size; 

            
            for (int local_row_idx = 0; local_row_idx < block_size; ++local_row_idx) {
                
                // Global row index in the original matrix
                int global_row_idx = block_row_idx * block_size + local_row_idx;
                
                double sum = 0.0;
                for (int local_col_idx = 0; local_col_idx < block_size; ++local_col_idx) {
                    
                    // Global column index in the original matrix
                    int global_col_idx = block_col_idx * block_size + local_col_idx;
                    
                    // Global value index in the values array
                    int global_value_idx = value_idx + (local_row_idx * block_size + local_col_idx);
                    sum += A.values[global_value_idx] * x[global_col_idx];
                    
                }
                
                y[global_row_idx] += sum;
            }
        }
    }
    
    return y;
}