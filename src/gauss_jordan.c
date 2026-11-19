#include "gauss_jordan.h"
#include "utils/matrix.h"
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

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
static inline void reduce_to_identity_matrix_(double *restrict matrix, int rows, int columns)
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

		// Ignores the diagonal of the row (since should be 1 and we will not utilizate any more), and multiply
		// on the rest of the row
		const double divisor = matrix[pivot_col * columns + pivot_col];
		for (int col = pivot_col + 1; col < columns; col++) {
			matrix[pivot_col * columns + col] /= divisor;
		}


		// Set column k to 0 from all rows by subtracting an appropriate multiple of row k.
		for (int row = 0; row < rows; row++) {
			if (row == pivot_col) {
				continue;
			}
			const double multiple = -matrix[row * columns + pivot_col];
			for (int i = pivot_col; i < columns; i++) {
				matrix[row * columns + i] += matrix[pivot_col * columns + i] * multiple;
			}
		}
	}
}

#define W 32
#define BLOCK_SIZE 32

static inline void update_rows_range(double *restrict matrix, int columns, int pc, int pend, int r0, int r1)
{
	const int bs = pend - pc;
	const double *P = matrix + (size_t)pc * columns;
	int r = r0;

	for (; r + 4 <= r1; r += 4) {
		double *restrict row0 = matrix + (size_t)(r + 0) * columns;
		double *restrict row1 = matrix + (size_t)(r + 1) * columns;
		double *restrict row2 = matrix + (size_t)(r + 2) * columns;
		double *restrict row3 = matrix + (size_t)(r + 3) * columns;

		double m0[BLOCK_SIZE], m1[BLOCK_SIZE], m2[BLOCK_SIZE], m3[BLOCK_SIZE];
		for (int j = 0; j < bs; j++) {
			m0[j] = row0[pc + j];
			row0[pc + j] = 0.0;
			m1[j] = row1[pc + j];
			row1[pc + j] = 0.0;
			m2[j] = row2[pc + j];
			row2[pc + j] = 0.0;
			m3[j] = row3[pc + j];
			row3[pc + j] = 0.0;
		}

		int i = pend;
		for (; i + W <= columns; i += W) {
			double a0[W] = {0}, a1[W] = {0}, a2[W] = {0}, a3[W] = {0};

			for (int j = 0; j < bs; j++) {
				const double *pj = P + (size_t)j * columns + i;
				for (int k = 0; k < W; k++) {
					const double p = pj[k];
					a0[k] += m0[j] * p;
					a1[k] += m1[j] * p;
					a2[k] += m2[j] * p;
					a3[k] += m3[j] * p;
				}
			}

			for (int k = 0; k < W; k++) {
				row0[i + k] -= a0[k];
				row1[i + k] -= a1[k];
				row2[i + k] -= a2[k];
				row3[i + k] -= a3[k];
			}
		}
		for (; i < columns; i++) {
			double a0 = 0, a1 = 0, a2 = 0, a3 = 0;
			for (int j = 0; j < bs; j++) {
				const double p = P[(size_t)j * columns + i];
				a0 += m0[j] * p;
				a1 += m1[j] * p;
				a2 += m2[j] * p;
				a3 += m3[j] * p;
			}
			row0[i] -= a0;
			row1[i] -= a1;
			row2[i] -= a2;
			row3[i] -= a3;
		}
	}

	for (; r < r1; r++) {
		double *row = matrix + (size_t)r * columns;
		double m[BLOCK_SIZE];
		for (int j = 0; j < bs; j++) {
			m[j] = row[pc + j];
			row[pc + j] = 0.0;
		}
		for (int i = pend; i < columns; i++) {
			double acc = 0.0;
			for (int j = 0; j < bs; j++)
				acc += m[j] * P[(size_t)j * columns + i];
			row[i] -= acc;
		}
	}
}

static inline void update_outside_rows(double *matrix, int rows, int columns, int pc, int pend)
{
	update_rows_range(matrix, columns, pc, pend, 0, pc);
	update_rows_range(matrix, columns, pc, pend, pend, rows);
}
static inline void reduce_to_identity_matrix(double *restrict matrix, int rows, int columns)
{

	for (int pc = 0; pc < (columns - 1); pc += BLOCK_SIZE) {
		for (int pivot_col = pc; pivot_col < MIN(pc + BLOCK_SIZE, (columns - 1)); pivot_col++) {
			int selected_row = pivot_col;
			double max_value = -DBL_MAX;

			// Partial pivoting (only in the block)
			for (int row = pivot_col; row < MIN(pc + BLOCK_SIZE, (columns - 1)); row++) {
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

			// Ignores the diagonal of the row (since should be 1 and we will not utilizate any more), and
			// multiply on the rest of the row
			const double divisor = matrix[pivot_col * columns + pivot_col];
			for (int col = pivot_col + 1; col < columns; col++) {
				matrix[pivot_col * columns + col] /= divisor;
			}

			// Set column k to 0 from rows of the block by subtracting an appropriate multiple of row k.
			for (int row = pc; row < MIN(pc + BLOCK_SIZE, (columns - 1)); row++) {
				if (row == pivot_col) {
					continue;
				}
				const double multiple = -matrix[row * columns + pivot_col];
				for (int i = pivot_col; i < columns; i++) {
					matrix[row * columns + i] += matrix[pivot_col * columns + i] * multiple;
				}
			}
		}
		// matrix_print(matrix, rows, columns);

		// Set column k to 0 from the rest of the rows by subtracting an appropriate multiple of row k.
		update_outside_rows(matrix, rows, columns, pc, MIN(pc + BLOCK_SIZE, (columns - 1)));
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
