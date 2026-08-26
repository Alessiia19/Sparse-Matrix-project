#include "matrix_coo.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

FormatCOO load_mtx(const std::string& filename) {
    std::ifstream file(filename);

    // Check the file availability
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file" << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    // Reads the first line (skipping comments)
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] != '%') break;
    }

    // Allocates memory space
    FormatCOO matrix;
    std::stringstream ss(line);
    ss >> matrix.num_rows >> matrix.num_cols >> matrix.nnz;

    matrix.row_indices.reserve(matrix.nnz);
    matrix.col_indices.reserve(matrix.nnz);
    matrix.values.reserve(matrix.nnz);

    // Put the values in the corresponding vectors
    int row, column;
    double value;
    for (int i = 0; i < matrix.nnz; ++i) {
        file >> row >> column >> value;
        matrix.row_indices.push_back(row - 1);
        matrix.col_indices.push_back(column - 1);
        matrix.values.push_back(value);
    }

    file.close();
    return matrix;
}

// Matrix-vector product
std::vector<double> spmv_coo(const FormatCOO& A, const std::vector<double>& x) {
    std::vector<double> y(A.num_rows, 0.0);

    for (int i = 0; i < A.nnz; ++i) {
        int row = A.row_indices[i];
        int column = A.col_indices[i];
        double value = A.values[i];
        
        y[row] += value * x[column];
    }

    return y;
}