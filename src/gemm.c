#include "gemm.h"
#include <stdlib.h>
#include <string.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* -------------------------------------------------------------------------
 * TODO (STUDENT): compute_XtX
 *
 * Compute XtX = X^T * X, where:
 *   X   is N x p (row-major): X[i*p + j] is element (i,j)
 *   XtX is p x p (row-major, caller-allocated): XtX[a*p + b] is element (a,b)
 *
 *   XtX[a][b] = sum over i=0..N-1 of X[i][a] * X[i][b]
 * Naive triple-nested loop. No blocking, no manual vectorization.
 * ---------------------------------------------------------------------- */
void compute_XtX_(const double *restrict X, double *restrict XtX, int N, int p)
{
	memset(XtX, 0, (size_t)p * (size_t)p * sizeof(*XtX));

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < p; j++) {
			for (int k = 0; k < p; k++) {
				XtX[j * p + k] += X[i * p + j] * X[i * p + k];
			}
		}
	}
}

void compute_XtX__(const double *restrict X, double *restrict XtX, int N, int p)
{
	const int BLOCK_SIZE = 32;

	memset(XtX, 0, (size_t)p * (size_t)p * sizeof(*XtX));

	for (int i = 0; i < N; i += BLOCK_SIZE) {
		for (int j = 0; j < p; j += BLOCK_SIZE) {
			for (int k = j; k < p; k += BLOCK_SIZE) {
				for (int ii = i; ii < MIN(i + BLOCK_SIZE, N); ++ii) {
					for (int jj = j; jj < MIN(j + BLOCK_SIZE, p); ++jj) {
						for (int kk = k; kk < MIN(k + BLOCK_SIZE, p); ++kk) {
							XtX[jj * p + kk] += X[ii * p + jj] * X[ii * p + kk];
						}
					}
				}
			}
		}
	}

	for (int j = 0; j < p; j++) {
		for (int k = j + 1; k < p; k++) {
			XtX[k * p + j] = XtX[j * p + k];
		}
	}
}


/* Other idea of computation, it exchange loops and performs other pattern */
void compute_XtX___(const double *restrict X, double *restrict XtX, int N, int p)
{
	memset(XtX, 0, (size_t)p * (size_t)p * sizeof(*XtX));

	const int BLOCK_SIZE = 32;

	for (int i = 0; i < p; i += BLOCK_SIZE) {
		for (int j = 0; j < N; j += BLOCK_SIZE) {
			for (int k = i; k < p; k += BLOCK_SIZE) {
				for (int ii = i; ii < MIN(i + BLOCK_SIZE, p); ++ii) {
					for (int jj = j; jj < MIN(j + BLOCK_SIZE, N); ++jj) {
						for (int kk = k; kk < MIN(k + BLOCK_SIZE, p); ++kk) {
							XtX[ii * p + kk] += X[jj * p + ii] * X[jj * p + kk];
						}
					}
				}
			}
		}
	}

	for (int j = 0; j < p; j++) {
		for (int k = j + 1; k < p; k++) {
			XtX[k * p + j] = XtX[j * p + k];
		}
	}
}


/* Primero traspone la matriz, asi simplemente lee una fila y empieza a multiplicar por las demas filas escribiendo en
 * memoria secuencialmente por cada fila.*/

void compute_XtX(const double *restrict Xt, double *restrict XtX, int N, int p)
{

	memset(XtX, 0, (size_t)p * p * sizeof(*XtX));

	const int BLOCK_SIZE = 32;
	const int BLOCK_SIZE_ROW = 150;

	for (int i = 0; i < p; i += BLOCK_SIZE) {
		for (int j = i; j < p; j += BLOCK_SIZE) {
			for (int k = 0; k < N; k += BLOCK_SIZE_ROW) {
				for (int ii = i; ii < MIN(i + BLOCK_SIZE, p); ++ii) {
					for (int jj = j; jj < MIN(j + BLOCK_SIZE, p); ++jj) {
						for (int kk = k; kk < MIN(k + BLOCK_SIZE_ROW, N); ++kk) {
							XtX[ii * p + jj] += Xt[ii * N + kk] * Xt[jj * N + kk];
						}
					}
				}
			}
		}
	}

	for (int j = 0; j < p; j++) {
		for (int k = j + 1; k < p; k++) {
			XtX[k * p + j] = XtX[j * p + k];
		}
	}
}
