#include "gaussian.h"
#include "utils/matrix.h"
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/**
 * Step 1 of Gaussian elimination, build an augmented p x (p+1) matrix [XtX | Xty]
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
 * Step 2 of Gaussian elimination, forward elimination: for each pivot column k = 0..p-1,
 *      a. partial pivoting: find the row r >= k with the largest
 *         absolute value in column k, and swap rows k and r if r != k
 *         (this avoids dividing by a very small/zero pivot).
 *      b. eliminate column k from all rows below k by subtracting an
 *         appropriate multiple of row k.
 *
 * @param[out] matrix  The augmented matrix [XtX | Xty]
 * @param[in]  rows    Number of rows of matrix
 * @param[in]  columns Number of columns of matrix
 *
 * NOTE: This functions ignore the case where matrix is singular
 */
static void forward_elimination(double *restrict matrix, int rows, int columns)
{
	const int BLOCK_SIZE = 32;

	// Last iteration is not necessary since there is not gonna be any swap or elimination (thats why (columns - 2))
	for (int pc = 0; pc < (columns - 2); pc += BLOCK_SIZE) {
		for (int pivot_col = pc; pivot_col < MIN(pc + BLOCK_SIZE, (columns - 2)); pivot_col++) {
			int selected_row = pivot_col;
			double max_value = -DBL_MAX;

			// Partial pivoting (only on the block range)
			for (int row = pivot_col; row < MIN(pc + BLOCK_SIZE, rows); row++) {
				if (fabs(matrix[row * columns + pivot_col]) > max_value) {
					selected_row = row;
					max_value = fabs(matrix[row * columns + pivot_col]);
				}
			}
			if (selected_row != pivot_col) {
				for (int i = pivot_col; i < columns;
				     i++) { // Columns left of pivot are zeros, no need to swap
					double tmp = matrix[pivot_col * columns + i];
					matrix[pivot_col * columns + i] = matrix[selected_row * columns + i];
					matrix[selected_row * columns + i] = tmp;
				}
			}

			// Add a multiple of selected row to each below row in block in order to eliminate all values
			// below pivot columns
			for (int row = pivot_col + 1; row < MIN(pc + BLOCK_SIZE, rows); row++) {
				const double multiple =
				    -(matrix[row * columns + pivot_col]) / matrix[pivot_col * columns + pivot_col];

				// matrix[row * columns + pivot_col] = 0.0; // Could be deleted since never readed
				for (int i = pivot_col + 1; i < columns; i++) {
					matrix[row * columns + i] += matrix[pivot_col * columns + i] * multiple;
				}
			}
		}

		// Add a multiple of selected row to each below row of block in order to eliminate all values
		// below pivot columns
		for (int row = pc + BLOCK_SIZE; row < rows; row++) {
			for (int pivot_col = pc; pivot_col < MIN(pc + BLOCK_SIZE, (columns - 2)); pivot_col++) {
				const double multiple =
				    -(matrix[row * columns + pivot_col]) / matrix[pivot_col * columns + pivot_col];

				// matrix[row * columns + pivot_col] = 0.0; // Could be deleted since never readed
				for (int i = pivot_col + 1; i < columns; i++) {
					matrix[row * columns + i] += matrix[pivot_col * columns + i] * multiple;
				}
			}
		}
	}
}

/**
 * Step 3 of Gaussian elimination, back substitution: once the augmented matrix is in upper
 * triangular form, solve for beta[p-1], beta[p-2], ..., beta[0] from the bottom row upward.
 *
 * @param[in]  matrix  The augmented matrix [XtX | Xty]
 * @param[in]  rows    Number of rows of matrix
 * @param[in]  columns Number of columns of matrix
 * @param[out] beta    The vector with the solution
 */
static inline void back_substitution(const double *restrict matrix, int rows, int columns, double *restrict beta)
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
void gaussian_elimination_solve(const double *restrict XtX, const double *restrict Xty, double *restrict beta, int p)
{

	const int rows = p;
	const int columns = p + 1;

	double *matrix = malloc((size_t)rows * (size_t)columns * sizeof(*matrix));
	if (!matrix) {
		fprintf(stderr, "Error allocating the augmented matrix [XtX | Xty]\n");
		exit(EXIT_FAILURE);
	}

	init_augmented_matrix(XtX, Xty, matrix, p);

	forward_elimination(matrix, rows, columns);

	back_substitution(matrix, rows, columns, beta);

	free(matrix);
}
