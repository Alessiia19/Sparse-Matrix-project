#include <iostream>
#include <vector>
#include "matrix_coo.hpp"
#include "matrix_csr.hpp"
#include "matrix_ell.hpp"
#include "matrix_bsr.hpp"

int main() {
    std::string filename = "test.mtx";
    FormatCOO A = load_mtx(filename);

    std::vector<double> x(A.num_cols, 1.0); 

    // --- COO ---   
    //std::vector<double> res = spmv_coo(A, x);
    //std::cout << "--- Result y = A * x (Format COO) ---" << std::endl;
    //for (size_t i = 0; i < res.size(); ++i) {
        //std::cout << "y[" << i << "] = " << res[i] << std::endl;
    //}

    // --- CSR ---
    //FormatCSR A_csr = convert_coo_to_csr(A);
    //std::vector<double> y_csr = spmv_csr(A_csr, x);
    //std::cout << "--- Result y = A * x (Format CSR) ---" << std::endl;
    //for (size_t i = 0; i < y_csr.size(); ++i) {
        //std::cout << "y[" << i << "] = " << y_csr[i] << std::endl;
        //}


    // -- ELLPACK --
    // FormatELL A_ell = convert_coo_to_ell(A);
    // std::vector<double> y_ell = spmv_ell(A_ell, x);

    /*std::cout << "--- Result y = A * x (Format ELL) ---" << std::endl;
    for (size_t i = 0; i < y_ell.size(); ++i) {
        std::cout << "y[" << i << "] = " << y_ell[i] << std::endl;
    }
    */

    MatrixBSR A_bsr = convert_coo_to_bsr(A, 1);
    std::vector<double> y_bsr = spmv_bsr(A_bsr, x);

    std::cout << "--- Result y = A * x (Format BSR) ---" << std::endl;
    for (size_t i = 0; i < y_bsr.size(); ++i) {
        std::cout << "y[" << i << "] = " << y_bsr[i] << std::endl;
    }

    return 0;


}