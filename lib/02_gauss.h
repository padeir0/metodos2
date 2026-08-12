#include "matrix.h"
#include "01_subst.h"

#include <math.h>
#include <float.h>

#ifndef M2_GAUSS_H
#define M2_GAUSS_H

int i_matrix_findFirstNonNullRow(const Matrix* A, int row_start, int column, double error) {
  int row = row_start;
  while (row < A->rows) {
    double value = matrix_at(A, row, column);
    if (fabs(value-0) > error) {
      return row;
    }
    row++;
  }
  return -1;
}

/* Here "sq" means "square" because this expects a square matrix.
   This function modifies both of it's pointer operands.
*/
bool matrix_sqGaussianElimination(Matrix* A, Matrix* B, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(A->columns == A->rows);
    assert(B->rows == A->columns);
    assert(B->columns == 1);
  #endif

  int k = 0;
  while (k < A->rows) {
    int nonNullRow = i_matrix_findFirstNonNullRow(A, k, k, error);

    if (nonNullRow == -1) {
      // the matrix is not invertible
      return false;
    }

    matrix_swapRows(A, k, nonNullRow);
    matrix_swapRows(B, k, nonNullRow);

    int i = k+1;
    double a_kk = matrix_at(A, k, k);
    while (i < A->rows) {
      double multiple = -(matrix_at(A, i, k) / a_kk);
      matrix_rowMultAdd(A, i, k, multiple);
      matrix_rowMultAdd(B, i, k, multiple);
      i++;
    }
    k++;
  }
  return true;
}
#endif
