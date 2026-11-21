# Task 1 — Sequential Baseline: Linear Regression via Normal Equations (Deliverable 1)

![Build](https://github.com/Abel-Breaker/C-Project-Starter/actions/workflows/build.yml/badge.svg)


This repository was created for the practical assignments of the High-Performance Computing Master's program at the University of Santiago de Compostela. The base repository for the base assignment is the following: [https://gitlab.citic.udc.es/emilio.padron/hpctools_linearregression](https://gitlab.citic.udc.es/emilio.padron/hpctools_linearregression?utm_source=chatgpt.com)


**The practical assignment consists of a 2 deliverables** based on the same codebase. This is the **second deliverable**.
 

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

Profile and optimize in FT3 the code presented in Task 1:
- Under the three scenarios considered in Task 1, regarding number of observations and numbre of variables.
- Using the same compilers than in Task 1.
- Using the best optimizations the compilers can apply for your code in FT3

## Benchmark results
You can see formatted benchmark results on `data_parser/benchs_results/task_2.txt`. The folder `data_parser/benchs_results/` has also the benchmark results in `.csv` format.

The raw outputs of the benchmark are in `sbatchs/outs/bench_task_2-11834967.out`.

## How to compile and execute?
The project has a `Makefile` to compile with a variety of different options (execute `make help` for more information). To execute the program you can simply run `./build/program 20000 50`, for example.

To automatically compile and execute the program you can use the `run.sh` script.

## How to recreate benchmark?
The benchmark results were obtained using the `sbatch` script provided in `sbatchs/sbatch.sh`.