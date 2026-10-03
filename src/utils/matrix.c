#include "matrix.h"
#include "terminal_formatting.h"
#include <stdio.h>

void matrix_print(const double *matrix, int rows, int columns)
{
	printf("%sMatrix\n%s", BOLD, COLOR_RESET);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			printf("%8.2f ", matrix[i * columns + j]);
		}
		printf("\n");
	}

	printf("\n");
}
