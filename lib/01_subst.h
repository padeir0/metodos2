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

#endif
