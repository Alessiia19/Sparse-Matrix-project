#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include "metrics.hpp"

// CSV export
void init_csv(const std::string& csv_filename);
void append_result_csv(const std::string& csv_filename, const BenchmarkResult& res);

// Calculate average nnz per row (for hybrid formats)
int get_k(const FormatCOO& coo);


#endif 