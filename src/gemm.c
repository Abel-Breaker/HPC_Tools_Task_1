#include "gemm.h"
#include <string.h>

/* -------------------------------------------------------------------------
 * TODO (STUDENT): compute_XtX
 *
 * Compute XtX = X^T * X, where:
 *   X   is N x p (row-major): X[i*p + j] is element (i,j)
 *   XtX is p x p (row-major, caller-allocated): XtX[a*p + b] is element (a,b)
 *
 *   XtX[a][b] = sum over i=0..N-1 of X[i][a] * X[i][b]
 * Naive triple-nested loop. No blocking, no manual vectorization.
 *
 * NOTE/TODO: The resultant matrix is symmetric. Many optimizations possible
 * ---------------------------------------------------------------------- */
void compute_XtX_old(const double *restrict X, double *restrict XtX, int N, int p)
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

/* Other idea of computation, it performs each superior row of XtX */
void compute_XtX_idea(const double *restrict X, double *restrict XtX, int N, int p)
{
	memset(XtX, 0, (size_t)p * (size_t)p * sizeof(*XtX));

	for (int i = 0; i < p; ++i) {
		for (int j = 0; j < N; ++j) {
			XtX[i * p + i] += X[j * p + i] * X[j * p + i];
			for (int k = i + 1; k < p; ++k) {
				XtX[i * p + k] += X[j * p + i] * X[j * p + k];
			}
		}
	}

	for (int j = 0; j < p; j++) {
		for (int k = j + 1; k < p; k++) {
			XtX[k * p + j] = XtX[j * p + k];
		}
	}
}

// TODO: Test blocking
void compute_XtX(const double *restrict X, double *restrict XtX, int N, int p)
{

	memset(XtX, 0, (size_t)p * p * sizeof(*XtX));

	// Matrix superior
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < p; j++) {
			for (int k = j; k < p; k++) {
				XtX[j * p + k] += X[i * p + j] * X[i * p + k];
			}
		}
	}


	// Matrix inferior
	for (int j = 0; j < p; j++) {
		for (int k = j + 1; k < p; k++) {
			XtX[k * p + j] = XtX[j * p + k];
		}
	}
}
