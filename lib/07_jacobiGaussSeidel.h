#include "matrix.h"

#include <float.h>

#ifndef M2_JACOBIGAUSSSEIDEL_H
#define M2_JACOBIGAUSSSEIDEL_H

bool matrix_jacobi(const Matrix* A, const Matrix* B, Matrix* X, int maxIter, double tolerance) {
  Matrix* NewX = matrix_new(X->rows, X->columns);
  Matrix* X_1 = X;
  Matrix* X_2 = NewX;

  int iter = 0;
  while (iter < maxIter) {
    int i = 0;
    while (i < A->rows) {
      double a_ii = matrix_at(A, i, i);
      double value = matrix_at(B, i, 0);
      int j = 0;
      while (j < A->columns) {
        if (i != j){
          double a_ij = matrix_at(A, i, j);
          double x_j = matrix_at(X_1, j, 0);
          value -= a_ij * x_j;
        }
        j++;
      }
      value = value / a_ii;
      matrix_setAt(X_2, i, 0, value);
      i++;
    }

    if (matrix_distanceSquared(X_1, X_2) < tolerance*tolerance) {
      break;
    }

    { // swap
      Matrix* Z = X_1;
      X_1 = X_2;
      X_2 = Z;
    }
    iter++;
  }

  bool out = true;
  // provavelmente atingiu maxiter antes atender à tolerancia
  if (matrix_distanceSquared(X_1, X_2) >= tolerance*tolerance) {
    out = false;
  }
  if (X != X_2) {
    matrix_copy(X_2, X);
  }
  matrix_free(&NewX);
  return out;
}

#endif
