#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef M2_SUBST_H
#define M2_SUBST_H

static inline
bool matrix_isLowerTriangular(const Matrix* A, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(error >= 0);
  #endif
  if (A->rows != A->columns) {
    return false;
  }

  int i; int j;

  i = 0;
  while (i < A->rows) {
    if (fabs(matrix_at(A, i, i) - 0) <= error) {
      return false;
    }
    j = i + 1;
    while (j < A->columns) {
      if (fabs(matrix_at(A, i, j) - 0) > error) {
        return false;
      }
      j++;
    }
    i++;
  }
  return true;
}

static inline
bool matrix_isUpperTriangular(const Matrix* A, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(error >= 0);
  #endif
  if (A->rows != A->columns) {
    return false;
  }

  int i; int j;

  i = 0;
  while (i < A->rows) {
    if (fabs(matrix_at(A, i, i) - 0) <= error) {
      return false;
    }
    j = i - 1;
    while (j >= 0) {
      if (fabs(matrix_at(A, i, j) - 0) > error) {
        return false;
      }
      j--;
    }
    i++;
  }
  return true;
}

static inline
void matrix_solveUpperTriangular(const Matrix* A, Matrix* X, const Matrix* B) {
  #if DEBUG
    assert(B->columns == 1);
    assert(B->rows == A->columns);
    assert(A->rows == A->columns);
  #endif
  
  int n = A->columns - 1;
  double sum = 0;
  double value = 0;

  value = matrix_at(B, n, 0) / matrix_at(A, n, n);
  matrix_setAt(X, n, 0, value);

  int k = n - 1;
  while (k >= 0) {
    sum = matrix_at(B, k, 0);
    int j = k+1;
    while (j <= n) {
      sum = sum - matrix_at(A, k, j)*matrix_at(X, j, 0);
      j++;
    }
    value = sum / matrix_at(A, k, k);
    matrix_setAt(X, k, 0, value);
    k--;
  }
}

static inline
void matrix_solveLowerTriangular(const Matrix* A, Matrix* X, const Matrix* B) {
  #if DEBUG
    assert(B->columns == 1);
    assert(B->rows == A->columns);
    assert(A->rows == A->columns);
  #endif
  
  int n = 0;
  double sum = 0;
  double value = 0;

  value = matrix_at(B, n, 0) / matrix_at(A, n, n);
  matrix_setAt(X, n, 0, value);

  int k = n + 1;
  while (k < A->rows) {
    int j = k-1;
    sum = matrix_at(B, k, 0);
    while (j >= 0) {
      sum = sum - matrix_at(A, k, j)*matrix_at(X, j, 0);
      j--;
    }
    value = sum / matrix_at(A, k, k);
    matrix_setAt(X, k, 0, value);
    k++;
  }
}

static inline
bool matrix_solveBySubstitution(const Matrix* A, Matrix* X, const Matrix* B, double error) {
  if (matrix_isUpperTriangular(A, error)) {
    matrix_solveUpperTriangular(A, X, B);
    return true;
  } else if (matrix_isLowerTriangular(A, error)) {
    matrix_solveLowerTriangular(A, X, B);
    return true;
  } else {
    return false;
  }
}

static inline
bool matrix_verifySolution(const Matrix* A, const Matrix* X, const Matrix* B, double error) {
  Matrix* result = matrix_new(B->rows, B->columns);
  matrix_mult(A, X, result);
  bool ok = matrix_equals(result, B, error);
  matrix_free(&result);
  return ok;
}

#endif
