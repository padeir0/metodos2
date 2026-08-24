#include "matrix.h"
#include "01_subst.h"
#include <float.h>

#ifndef M2_TRIDIAGONAL_H
#define M2_TRIDIAGONAL_H

/* Modifica os inputs! */
void matrix_solveTridiagonal(Matrix* A, Matrix* B, Matrix* C, Matrix* X, Matrix* D) {
  #if DEBUG
    assert(A != NULL); assert(B != NULL); assert(C != NULL);
    assert(X != NULL); assert(D != NULL);
    assert(A->columns == 1);
    assert(B->columns == 1);
    assert(C->columns == 1);
    assert(X->columns == 1);
    assert(D->columns == 1);

    assert(A->rows == B->rows);
    assert(B->rows == C->rows);
    assert(X->rows == D->rows);
  #endif

  int i = 1;
  while (i < A->rows) {
    double w = matrix_at(A, i, 0) / matrix_at(B, i-1, 0);
    double b_i = matrix_at(B, i, 0) - w * matrix_at(C, i-1, 0);
    double d_i = matrix_at(D, i, 0) - w * matrix_at(D, i-1, 0);

    matrix_setAt(B, i, 0, b_i);
    matrix_setAt(D, i, 0, d_i);

    i++;
  }

  int n = A->rows-1;

  double d_n = matrix_at(D, n, 0);
  double b_n = matrix_at(B, n, 0);
  double x_n = d_n / b_n;
  matrix_setAt(X, n, 0, x_n);

  i = n-1;
  while (i >= 0) {
    double d_i = matrix_at(D, i, 0);
    double c_i = matrix_at(C, i, 0);
    double b_i = matrix_at(B, i, 0);
    double x_iplus = matrix_at(X, i+1, 0);

    double x_i = (d_i - c_i * x_iplus) / b_i;
    matrix_setAt(X, i, 0, x_i);
    i--;
  }
}

#endif
