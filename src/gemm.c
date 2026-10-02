#include "gemm.h"

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
void compute_XtX(const double X[], double XtX[], int N, int p)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < p; j++) {
			for (int k = 0; k < p; k++) {
				XtX[j * p + k] += X[i * p + j] * X[i * p + k];
			}
		}
	}
}
