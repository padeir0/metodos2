#include "matrix.h"
#include "basicTypes.h"
#include "04_tridiagonal.h"

#include <float.h>

#ifndef M2_linearDiffEq_H
#define M2_linearDiffEq_H

/*
Discretiza uma EDO no formato:
  u''(t) = u(t) + v(t)u(t) + w(t)u'(t)
  x(a) = x_a
  x(b) = x_b

Onde `n` é o número de intervalos para discretizar

Os vetores A,B,C e D correspondem aos vetores de um sistema
tridiagonal e tem seus nomes análogos em 04_tridiagonal.h.
*/
static inline
void de_LinearBuildTriSys(RealFunction u, RealFunction v, RealFunction w,
                                   Matrix* A, Matrix* B, Matrix* C, Matrix* D,
                                   int n, double a, double b, double x_a, double x_b) {
  #if DEBUG
    assert(A != NULL); assert(B != NULL);
    assert(C != NULL); assert(D != NULL);
    assert(A->columns == 1);
    assert(B->columns == 1);
    assert(C->columns == 1);
    assert(D->columns == 1);
    assert(A->rows == B->rows);
    assert(B->rows == C->rows);
    assert(C->rows == D->rows);
    assert(n-1 == A->rows);
  #endif
  double h = (b-a)/n;
  double h_sq = h * h;

  int len = n-1;

  {
    double t_1 = a+h;

    matrix_setAt(A, 0, 0, 0);

    double b_1 = 2 + v(t_1) * h_sq;
    matrix_setAt(B, 0, 0, b_1);

    double c_1 = w(t_1) * h/2 -1;
    matrix_setAt(C, 0, 0, c_1);

    double d_1 = -h_sq*u(t_1) + x_a + x_a*(w(t_1)*h)/2;
    matrix_setAt(D, 0, 0, d_1);
  }

  int i = 1;
  while (i < len-1) {
    // j = i+1
    double t_j = a + (i+1) * h;

    double a_j = -(1 + (w(t_j) * h)/2);
    double b_j = 2 + v(t_j) * h_sq;
    double c_j = (w(t_j) * h)/2 - 1;
    double d_j = -h_sq * u(t_j);

    matrix_setAt(A, i, 0, a_j);
    matrix_setAt(B, i, 0, b_j);
    matrix_setAt(C, i, 0, c_j);
    matrix_setAt(D, i, 0, d_j);
    i++;
  }

  {
    // t_{n-1}
    double t_end = a+(n-1)*h;
    int i = len-1;

    double a_end = -(1 + w(t_end) * h/2);
    matrix_setAt(A, i, 0, a_end);

    double b_end = 2 + v(t_end) * h_sq;
    matrix_setAt(B, i, 0, b_end);

    matrix_setAt(C, i, 0, 0);

    double d_end = -h_sq*u(t_end) + x_b - x_b*(w(t_end)*h)/2;
    matrix_setAt(D, i, 0, d_end);
  }
}
#endif
