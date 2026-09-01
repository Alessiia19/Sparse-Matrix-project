#include "utils.hpp"
#include <iostream>
#include <fstream>

void init_csv(const std::string& csv_filename) {
    std::ofstream file(csv_filename, std::ios::trunc);
    if (file.is_open()) {
        file << "Matrix,Format,Memory_MB,Time_ms,GFLOPS,Bandwidth_GBs,Density\n";
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
             << res.density << "\n";
        file.close();
    }
}
