# Sparse-Matrix-project
A comparative study of various sparse matrix storage formats and their performance impact on SpMV operations across different architectural workloads.

## Overview
This repository contains experimental code to benchmark sparse matrix formats against hardware limitations. 
The project evaluates formats such as:
* **Baseline:** CSR (Compressed Sparse Row), ELLPACK
* **Hybrid/Advanced:** HYB (ELL + COO), BCSR (Blocked CSR)

## Test Matrices
We evaluate performance using datasets from the **SuiteSparse Matrix Collection**:
* **Structured:** `nos5`, `bcsstk13`, `plat362`
* **Unstructured / Graphs:** `fidapm05`, `wiki-Vote`, `west0497`
