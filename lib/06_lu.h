#include "matrix.h"
#include "01_subst.h"

#include <math.h>
#include <float.h>

#ifndef M2_LU_H
#define M2_LU_H

static inline
bool matrix_NaiveLUDecomposition(const Matrix* A, Matrix* L, Matrix* U, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(L != NULL);
    assert(U != NULL);
    assert(matrix_isSquare(A));
    assert(matrix_sameShape(A, L));
    assert(matrix_sameShape(A, U));
  #endif

  matrix_setIdentity(L);
  matrix_copy(A, U);

  int k = 0;
  while (k < U->rows) {
    double u_kk = matrix_at(U, k, k);
    if (fabs(u_kk - 0) < error) {
      // não fazemos pivotamento
      return false;
    }
    int i = k+1;
    while (i < U->rows) {
      double multiple = matrix_at(U, i, k) / u_kk;
      matrix_rowMultAdd(U, i, k, -multiple);
      matrix_setAt(L, i, k, multiple);
      i++;
    }
    k++;
  }
  return true;
}

static inline
bool matrix_NaiveLDLDecomposition(Matrix* A, Matrix* L, Matrix* D, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(L != NULL);
    assert(D != NULL);
    assert(matrix_isSquare(A));
    assert(matrix_sameShape(A, L));
    assert(matrix_sameShape(A, D));
  #endif

  matrix_setIdentity(L);
  matrix_setIdentity(D);

  int k = 0;
  while (k < A->rows) {
    double a_kk = matrix_at(A, k, k);
    if (fabs(a_kk - 0) < error) {
      // não fazemos pivotamento
      return false;
    }
    matrix_setAt(D, k, k, a_kk);    

    int i = k+1;
    while (i < A->rows) {
      double multiple = matrix_at(A, i, k) / a_kk;
      matrix_rowMultAdd(A, i, k, -multiple);
      matrix_setAt(L, i, k, multiple);
      i++;
    }
    k++;
  }
  return true;
}

static inline
int i_matrix_findLargestRow(const Matrix* A, int row_start, int column, double error) {
  // usamos apenas valores em módulo, o minimo é zero
  double largest = 0;
  int selected = row_start;
  int row = row_start;
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

static inline
bool matrix_PivotingLUDecomposition(const Matrix* A, Matrix* P, Matrix* L, Matrix* U, double error) {
  #if DEBUG
    assert(P != NULL);
    assert(A != NULL);
    assert(L != NULL);
    assert(U != NULL);
    assert(matrix_isSquare(A));
    assert(matrix_sameShape(A, L));
    assert(matrix_sameShape(A, U));
    assert(matrix_sameShape(A, P));
  #endif

  matrix_setIdentity(L);
  matrix_setIdentity(P);
  matrix_copy(A, U);

  int k = 0;
  while (k < U->rows) {
    int bestRow = i_matrix_findLargestRow(U, k, k, error);

    if (bestRow == -1) {
      // the matrix is not invertible
      return false;
    }

    matrix_swapRows(U, k, bestRow);
    matrix_swapRows(P, k, bestRow);
    // troca até a diagonal
    matrix_partialSwapRows(L, k, bestRow, k);

    double u_kk = matrix_at(U, k, k);
    int i = k+1;
    while (i < U->rows) {
      double multiple = matrix_at(U, i, k) / u_kk;
      matrix_rowMultAdd(U, i, k, -multiple);
      matrix_setAt(L, i, k, multiple);
      i++;
    }
    k++;
  }
  return true;
}
#endif
