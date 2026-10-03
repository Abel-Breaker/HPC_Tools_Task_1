#include "gaussian.h"
#include "utils/matrix.h"
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *   1. Build an augmented p x (p+1) matrix [XtX | Xty] (work on a local
 *      copy — do not modify XtX/Xty in place, you may want to keep them
 *      for the report).
 */
static inline void init_augmented_matrix(const double *XtX, const double *Xty, double *matrix, int p)
{
	const int rows = p;
	const int columns = p + 1;

	for (int i = 0; i < rows; i++) {
		int j;
		for (j = 0; j < (columns - 1); j++) {
			matrix[i * columns + j] = XtX[i * p + j];
		}
		matrix[(i * columns) + j] = Xty[i];
	}
}


/**
 *   2. Forward elimination: for each pivot column k = 0..p-1,
 *        a. partial pivoting: find the row r >= k with the largest
 *           absolute value in column k, and swap rows k and r if r != k
 *           (this avoids dividing by a very small/zero pivot).
 *        b. eliminate column k from all rows below k by subtracting an
 *           appropriate multiple of row k.
 */
static inline void forward_elimination(double *matrix, int rows, int columns)
{
	// Last iteration is not necessary since there is not gonna be any swap (thats why (columns - 2))
	for (int pivot = 0; pivot < (columns - 2); pivot++) {
		int selected_row = pivot;
		double max_value = -DBL_MAX;

		// Partial pivoting
		for (int row = pivot; row < rows; row++) {
			if (fabs(matrix[row * columns + pivot]) > max_value) {
				selected_row = row;
				max_value = fabs(matrix[row * columns + pivot]);
			}
		}
		if (selected_row != pivot) {
			for (int i = pivot; i < columns; i++) { // Columns left of pivot are zeros, no need to swap
				double tmp = matrix[pivot * columns + i];
				matrix[pivot * columns + i] = matrix[selected_row * columns + i];
				matrix[selected_row * columns + i] = tmp;
			}
		}

		//  Eliminate column k from all rows below k
		for (int row = pivot + 1; row < rows; row++) {
			const double multiple = -(matrix[row * columns + pivot]) / matrix[pivot * columns + pivot];

			matrix[row * columns + pivot] = 0.0; // Could be deleted
			for (int i = pivot + 1; i < columns; i++) {
				matrix[row * columns + i] += matrix[pivot * columns + i] * multiple;
			}
		}
	}
}

/**
 *   3. Back substitution: once the augmented matrix is in upper
 *      triangular form, solve for beta[p-1], beta[p-2], ..., beta[0]
 *      from the bottom row upward.
 */
static inline void back_substitution(const double *matrix, int rows, int columns, double *beta)
{
	for (int row = rows - 1; row >= 0; row--) {
		double sum = 0;
		for (int i = rows - 1; i > row; i--) {
			sum += beta[i] * matrix[row * columns + i];
		}

		beta[row] = (matrix[row * columns + columns - 1] - sum) / matrix[row * columns + row];
	}
}


/* -------------------------------------------------------------------------
 * TODO (STUDENT): gaussian_elimination_solve
 *
 * Solve the p x p system:
 *
 *   XtX * beta = Xty
 *
 * using Gaussian elimination with partial pivoting, followed by back
 * substitution:
 *
 *   1. Build an augmented p x (p+1) matrix [XtX | Xty] (work on a local
 *      copy — do not modify XtX/Xty in place, you may want to keep them
 *      for the report).
 *   2. Forward elimination: for each pivot column k = 0..p-1,
 *        a. partial pivoting: find the row r >= k with the largest
 *           absolute value in column k, and swap rows k and r if r != k
 *           (this avoids dividing by a very small/zero pivot).
 *        b. eliminate column k from all rows below k by subtracting an
 *           appropriate multiple of row k.
 *   3. Back substitution: once the augmented matrix is in upper
 *      triangular form, solve for beta[p-1], beta[p-2], ..., beta[0]
 *      from the bottom row upward.
 *
 * XtX  : p x p, row-major (read-only)
 * Xty  : p (right-hand side, read-only)
 * beta : p (output, caller-allocated)
 * ---------------------------------------------------------------------- */
void gaussian_elimination_solve(const double *XtX, const double *Xty, double *beta, int p)
{

	const int rows = p;
	const int columns = p + 1;

	double *matrix = malloc((size_t)rows * (size_t)columns * sizeof(*matrix));
	if (!matrix) {
		fprintf(stderr, "Error\n");
		exit(EXIT_FAILURE);
	}

	init_augmented_matrix(XtX, Xty, matrix, p);

	forward_elimination(matrix, rows, columns);

	back_substitution(matrix, rows, columns, beta);

	free(matrix);
}
