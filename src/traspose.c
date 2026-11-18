#include "traspose.h"
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

void traspose(const double *restrict X, double *restrict Xt, int N, int p)
{
	const int BLOCK_SIZE = 32;

	for (int i = 0; i < p; i += BLOCK_SIZE) {
		for (int j = 0; j < N; j += BLOCK_SIZE) {
			for (int ii = i; ii < MIN(i + BLOCK_SIZE, p); ii++) {
				for (int jj = j; jj < MIN(j + BLOCK_SIZE, N); jj++) {
					Xt[ii * N + jj] = X[jj * p + ii];
				}
			}
		}
	}
}
