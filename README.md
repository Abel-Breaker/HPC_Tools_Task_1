# Task 1 — Sequential Baseline: Linear Regression via Normal Equations (Deliverable 1)

![Build](https://github.com/Abel-Breaker/C-Project-Starter/actions/workflows/build.yml/badge.svg)


This repository was created for the practical assignments of the High-Performance Computing Master's program at the University of Santiago de Compostela. The base repository for the assignment is the following: [https://gitlab.citic.udc.es/emilio.padron/hpctools_linearregression](https://gitlab.citic.udc.es/emilio.padron/hpctools_linearregression?utm_source=chatgpt.com)


The practical assignment consists of a series of deliverables based on the same codebase. **Each deliverable is identified by a specific tag in the repository**, corresponding to the assignment to be submitted. (**Current tag: Sequential Baseline (deliverable 1)**)

## Context

Given a dataset of `N` observations, each described by `p` predictor variables, linear regression seeks the coefficient vector `β` (size `p`) that best fits the data in a least-squares sense:

```
minimize  ||Xβ - y||²
```

where `X` is the `N × p` matrix of observations and `y` is the `N × 1` vector of target values. This problem has a closed-form solution obtained by setting the gradient to zero, which leads to the **normal equations**:

```
(XᵀX) β = Xᵀy
```

`XᵀX` is a `p × p` symmetric positive-definite matrix, and `Xᵀy` is a `p × 1` vector. Solving for `β` therefore requires two steps:

1. **Matrix multiplication**: compute `XᵀX` (and `Xᵀy`).
2. **Linear system solve**: solve the resulting `p × p` system for `β`.

## Task

Implement a **sequential, non-optimized** baseline version of this linear regression pipeline: naive (ijk) matrix multiplication for `XᵀX`/`Xᵀy`, followed by Gaussian elimination (with partial pivoting) and back substitution to solve the resulting system. No blocking, vectorization, or parallelization: clarity and correctness first. This baseline is the reference for later optimization stages.

Add (and try) an alternative solver for the linear equation system, using Gauss-Jordan instead of simple Gaussian elimination. Modify the code to allow the switching between both (non-optimizes so far) solvers.

A routine that generates synthetic data is provided on the codebase, with: a random `X`, a known ground-truth `β_true`, and `y = X·β_true + noise`. This lets verify the computed `β` against the known ground truth.

You need to do a benchmarking report on the FinisTerrae III (the CESGA supercomputer) to compare the performance of binaries compiled with `gcc-10.1.0`, `icc 2021.3.0`, and `icx 2021.3.0` using `-O0`, `-O2`, `-O3`, and `-Ofast` (for optimization levels above `-O0`, `-march=native` will also be enabled). For each configuration, results will be verified against the required tolerance, and execution time will be averaged over multiple runs. The benchmarks will use the following problem sizes: $(N=20000, p=50)$, $(N=50000, p=300)$, and $(N=2000, p=2000)$.

## Benchmark results
You can see formatted benchmark results on `data_parser/benchs_results/task_1.txt`. The folder `data_parser/benchs_results/` has also the benchmark results in `.csv` format.

The raw outputs of the benchmark are in `sbatchs/outs/bench_task_1-10311140.out`.

## How to compile and execute?
The project has a `Makefile` to compile with a variety of different options (execute `make help` for more information). To execute the program you can simply run `./build/program 20000 50`, for example.

To automatically compile and execute the program you can use the `run.sh` script.

## How to recreate benchmark?
The benchmark results were obtained using the `sbatch` script provided in `sbatchs/sbatch.sh`.