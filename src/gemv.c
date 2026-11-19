#include "gemv.h"
#include <stdlib.h>
#include <string.h>

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
	memset(Xty, 0, (size_t)p * sizeof(*Xty));

	for (int i = 0; i < p; i++) {
		for (int j = 0; j < N; ++j) {
			Xty[i] += Xt[i * N + j] * y[j];
		}
	}
}
