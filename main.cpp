#include <iostream>
#include <vector>
#include "matrix_coo.hpp"

int main() {
    std::string filename = "test.mtx";
    FormatCOO A = load_mtx(filename);

    std::vector<double> x(A.num_cols, 1.0); 
    std::vector<double> res = spmv_coo(A, x);

    std::cout << "--- Result y = A * x (Format COO) ---" << std::endl;
    for (size_t i = 0; i < res.size(); ++i) {
        std::cout << "y[" << i << "] = " << res[i] << std::endl;
    }

    return 0;
}