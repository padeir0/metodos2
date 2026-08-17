#include "matrix.h"
#include "01_subst.h"

#include <math.h>
#include <float.h>

#ifndef M2_GAUSSPIVOT_H
#define M2_GAUSSPIVOT_H

/*
   Aqui eu implementei dois algoritmos: o do livro da Cristina Cunha (terminado em _1)
   e o dado em sala (terminado em _2).
*/
static inline
int i_matrix_findLargestRow(const Matrix* A, int row_start, int column, double error) {
  int row = row_start;
  // usamos apenas valores em módulo, o minimo é zero
  double largest = 0;
  int selected = row_start;
  while (row < A->rows) {
    double value = fabs(matrix_at(A, row, column));
    if (largest < value) {
      largest = value;
      selected = row;
    }
    row++;
  }
  if (fabs(largest-0) < error) {
    return -1;
  }
  return selected;
}

/* Here "sq" means "square" because this expects a square matrix.
   This function modifies both of it's pointer operands.
*/
static inline
bool matrix_sqGaussianEliminationPivot_1(Matrix* A, Matrix* B, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(A->columns == A->rows);
    assert(B->rows == A->columns);
    assert(B->columns == 1);
  #endif

  int k = 0;
  while (k < A->rows) {
    int bestRow = i_matrix_findLargestRow(A, k, k, error);

    if (bestRow == -1) {
      // the matrix is not invertible
      return false;
    }

    matrix_swapRows(A, k, bestRow);
    matrix_swapRows(B, k, bestRow);

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

/* Returns a vector (Nx1 matrix) containing the largest element of each row.
*/
static inline
Matrix* i_matrix_createMaxArray(Matrix* A) {
  int i = 0;
  int j = 0;

  Matrix* out = matrix_new(A->rows, 1);

  while (i < A->rows) {
    double largest = DBL_MIN;
    while (j < A->columns) {
      double value = matrix_at(A, i, j);
      if (largest < value) {
        largest = value;
      }
      j++;
    }
    matrix_setAt(out, i, 0, largest);
    i++;
  }
  return out;
}

static inline
int i_matrix_findLargestRelativeRow(const Matrix* A, const Matrix* maxMatrix, int row_start, int column, double error) {
  int row = row_start;
  int selected = row_start;
  // R só assume valores positivos, então o mínimo é 0
  double largestR = 0;

  while (row < A->rows) {
    double a_ik = matrix_at(A, row, column);
    double s_i = matrix_at(maxMatrix, row, 0);
    double R = fabs(a_ik/s_i);
    if (largestR < R && R != 0) {
      largestR = R;
      selected = row;
    }
    row++;
  }
  if (fabs(largestR - 0) < error) {
    return -1;
  }
  return selected;
}

/* Here "sq" means "square" because this expects a square matrix.
   This function modifies both of it's pointer operands.
   THIS FUNCTION ALLOCATES!
*/
static inline
bool matrix_sqGaussianEliminationPivot_2(Matrix* A, Matrix* B, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(A->columns == A->rows);
    assert(B->rows == A->columns);
    assert(B->columns == 1);
  #endif

  Matrix* maxMatrix = i_matrix_createMaxArray(A);

  int k = 0;
  while (k < A->rows) {
    int bestRow = i_matrix_findLargestRelativeRow(A, maxMatrix, k, k, error);
    if (bestRow == -1) {
      // the matrix is not invertible
      matrix_free(&maxMatrix);
      return false;
    }

    matrix_swapRows(A, k, bestRow);
    matrix_swapRows(B, k, bestRow);

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

  matrix_free(&maxMatrix);
  return true;
}

#endif
