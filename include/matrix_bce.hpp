#ifndef MATRIX_BCE_HPP
#define MATRIX_BCE_HPP

#include <vector>
#include "matrix_coo.hpp"
#include "matrix_ell.hpp"
#include "matrix_csr.hpp"

struct FormatBCE {
    int num_rows = 0;
    int num_cols = 0;

    FormatELL ell; 
    FormatCSR csr; 
};


FormatBCE convert_coo_to_bce(const FormatCOO& coo);
std::vector<double> spmv_bce(const FormatBCE& bce, const std::vector<double>& x);

#endif 