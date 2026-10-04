#include "gauss_jordan.h"
#include "utils/matrix.h"
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Step 1 of Gauss-Jordan, build an augmented p x (p+1) matrix [XtX | Xty]
 *
 * @param[in]  XtX    XtX = X^T * X. It is a p x p matrix in row-major
 * @param[in]  Xty    Xty = X^T * y. It is a p x 1 matrix right-hand sider
 * @param[out] matrix The augmented matrix [XtX | Xty]
 * @param[in]  p      The dimensions of the original matrices
 */
static inline void init_augmented_matrix(const double *restrict XtX, const double *restrict Xty,
					 double *restrict matrix, int p)
{
	const int rows = p;
	const int columns = p + 1;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < (columns - 1); j++) {
			matrix[i * columns + j] = XtX[i * p + j];
		}
		matrix[(i * columns) + (columns - 1)] = Xty[i]; // Fill [Xty]
	}
}


/**
 * Step 2 of Gauss-Jordan, reduce to identity matrix: for each pivot column k = 0..p-1,
 *      a. partial pivoting: find the row r >= k with the largest
 *         absolute value in column k, and swap rows k and r if r != k
 *         (this avoids dividing by a very small/zero pivot).
 *      b. Set column k to 0 from all rows by subtracting an appropriate multiple of row k
 *         (except diagonal to 1).
 *
 * @param[out] matrix  The augmented matrix [XtX | Xty]
 * @param[in]  rows    Number of rows of matrix
 * @param[in]  columns Number of columns of matrix
 *
 * NOTE: This functions ignore the case where matrix is singular
 */
static inline void reduce_to_identity_matrix(double *restrict matrix, int rows, int columns)
{
	for (int pivot_col = 0; pivot_col < (columns - 1); pivot_col++) {
		int selected_row = pivot_col;
		double max_value = -DBL_MAX;

		// Partial pivoting
		for (int row = pivot_col; row < rows; row++) {
			if (fabs(matrix[row * columns + pivot_col]) > max_value) {
				selected_row = row;
				max_value = fabs(matrix[row * columns + pivot_col]);
			}
		}
		if (selected_row != pivot_col) {
			for (int i = pivot_col; i < columns; i++) { // Columns left of pivot are zeros, no need to swap
				double tmp = matrix[pivot_col * columns + i];
				matrix[pivot_col * columns + i] = matrix[selected_row * columns + i];
				matrix[selected_row * columns + i] = tmp;
			}
		}

		// Set the diagonal of the row to 1 (and multiply on the rest of the row)
		const double divisor = matrix[pivot_col * columns + pivot_col];
		for (int col = pivot_col; col < columns; col++) {
			matrix[pivot_col * columns + col] /= divisor;
		}

		// Set column k to 0 from all rows above k by subtracting an appropriate multiple of row k.
		for (int row = 0; row < pivot_col; row++) {
			const double multiple = -matrix[row * columns + pivot_col];
			for (int i = pivot_col; i < columns; i++) {
				matrix[row * columns + i] += matrix[pivot_col * columns + i] * multiple;
			}
		}

		// Set column k to 0 from all rows below k by subtracting an appropriate multiple of row k.
		for (int row = pivot_col + 1; row < rows; row++) {
			const double multiple = -matrix[row * columns + pivot_col];
			for (int i = pivot_col; i < columns; i++) {
				matrix[row * columns + i] += matrix[pivot_col * columns + i] * multiple;
			}
		}
	}
}

/**
 * Step 3 of Gauss-Jordan, the last column of the matrix corresponds to the vector beta
 *
 * @param[in]  matrix  The augmented matrix [XtX | Xty]
 * @param[in]  rows    Number of rows of matrix
 * @param[in]  columns Number of columns of matrix
 * @param[out] beta    The vector with the solution
 */
static inline void obtain_beta(const double *restrict matrix, int rows, int columns, double *restrict beta)
{
	for (int row = 0; row < rows; row++) {
		beta[row] = matrix[row * columns + columns - 1];
	}
}


/* -------------------------------------------------------------------------
 * TODO (STUDENT): Gauss-Jordan
 *
 * Solve the p x p system:
 *
 *   XtX * beta = Xty
 *
 * using Gauss-Jordan with partial pivoting.
 *
 *   1. Build an augmented p x (p+1) matrix [XtX | Xty] (work on a local
 *      copy — do not modify XtX/Xty in place, you may want to keep them
 *      for the report).
 *   2. Reduce to identity matrix: for each pivot column k = 0..p-1,
 *        a. partial pivoting: find the row r >= k with the largest
 *           absolute value in column k, and swap rows k and r if r != k
 *           (this avoids dividing by a very small/zero pivot).
 *        b. Set column k to 0 from all rows by subtracting an appropriate multiple of row k
 *           (except diagonal to 1).
 *   3. The last column of the matrix corresponds to the vector beta.
 *
 * XtX  : p x p, row-major (read-only)
 * Xty  : p (right-hand side, read-only)
 * beta : p (output, caller-allocated)
 * ---------------------------------------------------------------------- */
void gauss_jordan_solve(const double *restrict XtX, const double *restrict Xty, double *restrict beta, int p)
{

	const int rows = p;
	const int columns = p + 1;

	double *matrix = malloc((size_t)rows * (size_t)columns * sizeof(*matrix));
	if (!matrix) {
		fprintf(stderr, "Error allocating the augmented matrix [XtX | Xty]\n");
		exit(EXIT_FAILURE);
	}

	init_augmented_matrix(XtX, Xty, matrix, p);

	reduce_to_identity_matrix(matrix, rows, columns);

	obtain_beta(matrix, rows, columns, beta);

	free(matrix);
}
