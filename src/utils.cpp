#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

void init_csv(const std::string& csv_filename) {
    std::ofstream file(csv_filename, std::ios::trunc);
    if (file.is_open()) {
        file << "Matrix,Format,Memory_MB,Time_ms,GFLOPS,Bandwidth_GBs,Allocation_Ratio\n";
        file.close();
    }
}

void append_result_csv(const std::string& csv_filename, const BenchmarkResult& res) {
    std::ofstream file(csv_filename, std::ios::app);
    if (file.is_open()) {
        file << res.matrix << ","
             << res.format << ","
             << res.memory_megabytes << ","
             << res.time_ms << ","
             << res.gflops << ","
             << res.bandwidth_gbs << ","
             << res.allocation_ratio << "\n";
        file.close();
    }
}

int get_k(const FormatCOO& coo) {
    double average_nnz_per_row = static_cast<double>(coo.nnz) / coo.num_rows;
    return static_cast<int>(std::ceil(average_nnz_per_row));
}
