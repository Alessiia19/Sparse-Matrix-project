#include "matrix_coo.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

FormatCOO load_matrix(const std::string& filename) {
    std::ifstream file(filename);

    // Check the file availability
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file" << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    // Reads the header
    std::string line;
    bool is_symmetric = false;
    bool is_pattern = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue; 

        if (line.find("symmetric") != std::string::npos) is_symmetric = true;
        if (line.find("pattern") != std::string::npos) is_pattern = true;

        if (line[0] != '%') break;
    }

    // Allocates memory space
    FormatCOO matrix;
    std::stringstream ss(line);
    ss >> matrix.num_rows >> matrix.num_cols >> matrix.nnz;
    
    size_t estimated_nnz; 
    if (is_symmetric) {
        estimated_nnz = static_cast<size_t>(matrix.nnz) * 2;
    } else {
        estimated_nnz = static_cast<size_t>(matrix.nnz);
    }

    matrix.row_indices.reserve(estimated_nnz);
    matrix.col_indices.reserve(estimated_nnz);
    matrix.values.reserve(estimated_nnz);

    // Put the values in the corresponding vectors
    int row, column;
    double value;
    while (file >> row >> column) {
        if (!is_pattern) {
            file >> value;
        } else {
            value = 1.0; 
        }

        int r = row - 1;
        int c = column - 1;

        matrix.row_indices.push_back(r);
        matrix.col_indices.push_back(c);
        matrix.values.push_back(value);

        if (is_symmetric && r != c) {
            matrix.row_indices.push_back(c);
            matrix.col_indices.push_back(r);
            matrix.values.push_back(value);
        }
    }

    matrix.nnz = static_cast<int>(matrix.values.size());
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