#ifndef MATRIX_HYB_HPP
#define MATRIX_HYB_HPP

#include <vector>
#include "matrix_coo.hpp"
#include "matrix_ell.hpp"

struct FormatHYB {
    int num_rows = 0;
    int num_cols = 0;
    
    FormatELL ell;
    FormatCOO coo;
};

int get_k(const FormatCOO& coo);
FormatHYB convert_coo_to_hyb(const FormatCOO& coo);
std::vector<double> spmv_hyb(const FormatHYB& A, const std::vector<double>& x);

#endif