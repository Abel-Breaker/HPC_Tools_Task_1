#include "gemv.h"
/* -------------------------------------------------------------------------
 * TODO (STUDENT): compute_Xty
 *
 * Compute Xty = X^T * y, where:
 *   X   is N x p (row-major)
 *   y   is N     (vector)
 *   Xty is p     (output, caller-allocated)
 *
 *   Xty[a] = sum over i=0..N-1 of X[i][a] * y[i]
 * ---------------------------------------------------------------------- */
void compute_Xty(const double *restrict Xt, const double *restrict y, double *restrict Xty, int N, int p)
{
	for (int i = 0; i < p; i++) {
		Xty[i] = 0;
		for (int j = 0; j < N; ++j) {
			Xty[i] += Xt[i * N + j] * y[j];
		}
	}
}
