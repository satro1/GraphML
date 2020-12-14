#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

double **alloc_2d_array(int rows, int cols) {
  double **arr;
  arr = (double **) calloc(rows, sizeof(double *));
  #pragma omp parallel for
  for (int i = 0; i < rows; i++) {
    arr[i] = (double *)calloc(cols, sizeof(double));
  }
  return arr;
}