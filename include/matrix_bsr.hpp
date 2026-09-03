#ifndef MATRIX_BSR_HPP
#define MATRIX_BSR_HPP

#include <vector>
#include "matrix_coo.hpp"

struct FormatBSR {
    int num_rows = 0;
    int num_cols = 0;

    int num_rows_with_padding = 0;  
    int num_cols_with_padding = 0;

    int block_size = 1;       
    int num_block_rows = 0;   
    int num_block_cols = 0;   
    int nnz_blocks = 0;       

    std::vector<int> pointerB;       
    std::vector<int> pointerE;       
    std::vector<int> columns; 
    std::vector<double> values;   
};

FormatBSR convert_coo_to_bsr(const FormatCOO& coo, int block_size);
std::vector<double> spmv_bsr(const FormatBSR& A, const std::vector<double>& x);

#endif