#include <iostream>
#include <vector>
#include "matrix_coo.hpp"
#include "matrix_csr.hpp"

int main() {
    std::string filename = "test.mtx";
    FormatCOO A = load_mtx(filename);

    std::vector<double> x(A.num_cols, 1.0); 

    // moltiplicazione di coo per il vettore 
    //std::vector<double> res = spmv_coo(A, x);

    // Conversione ed esecuzione CSR
    MatrixCSR A_csr = convert_coo_to_csr(A);
    std::vector<double> y_csr = spmv_csr(A_csr, x);

    //std::cout << "--- Result y = A * x (Format COO) ---" << std::endl;
    //for (size_t i = 0; i < res.size(); ++i) {
        //std::cout << "y[" << i << "] = " << res[i] << std::endl;
    //}

    std::cout << "--- Risult y = A * x (Format CSR) ---" << std::endl;
    for (size_t i = 0; i < y_csr.size(); ++i) {
        std::cout << "y[" << i << "] = " << y_csr[i] << std::endl;
    }

    return 0;


}