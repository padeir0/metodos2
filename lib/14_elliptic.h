#include "matrix.h"
#include "basicTypes.h"

#include <float.h>

#ifndef M2_elliptic_H
#define M2_elliptic_H

double de_ellipticIntervalWidth(double a, double b, double n) {
  return (b-a)/(n-1);
}

int de_seidelElliptic(
  Matrix* u,
  Real2Function f, Real2Function g,
  double a_x, double a_y, double h,
  int n_x, int n_y,
  int itermax, double tol
) {
  int k = 0;
  while (k < itermax) {
    int i = 1;
    double maxDelta = 0;
    while (i < n_x-1) {
      int j = 1;
      double x = a_x + h * i;
      while (j < n_y-1) {
        double y = a_y + h * j;
        double v = matrix_at(u, i+1, j) + matrix_at(u, i-1, j)
                   + matrix_at(u, i, j+1) + matrix_at(u, i, j-1);
        double u_ij = (v - h*h *g(x, y)) / (4 - h*h*f(x,y));

        double delta = fabs(u_ij - matrix_at(u,i,j));
        if (delta > maxDelta) maxDelta = delta;

        matrix_setAt(u, i, j, u_ij);
        j++;
      }
      i++;
    }
    if (maxDelta < tol) {
      return k;
    }
    k++;
  }
  return k;
}

/*
Soluciona uma EDP no formato:
    u_{xx}(x, y) + u_{yy}(x, y) + f(x,y)*u = g(x, y)
    u(x, y) = r(x, y) (em [a_x, b_x] x [a_y, b_y])

Onde f, g e r são funções dadas e a_x, a_y, b_x, b_y são números dados
que representam os vértices do retangulo que define a fronteira de u.
*/
int de_solveElliptic(
  Matrix** output,
  Real2Function f, Real2Function g, Real2Function r,
  Real2Function interior,
  double a_x, double b_x, double a_y, double b_y,
  int n_x, int n_y,
  int itermax, double tol
) {
  Matrix* u = matrix_new(n_x, n_y);
  if (u == NULL) {
    return -1;
  }
  *output = u;

  double h = (b_x - a_x)/(n_x-1);
  // inicializa a fronteira
  int j = 0;
  while (j < n_y) {
    double y = a_y + j * h;
    matrix_setAt(u, 0, j, r(a_x, y));
    matrix_setAt(u, n_x-1, j, r(b_x, y));
    j++;
  }
  int i = 0;
  while (i < n_x) {
    double x = a_x + i * h;
    matrix_setAt(u, i, 0, r(x, a_y));
    matrix_setAt(u, i, n_y-1, r(x, b_y));
    i++;
  }

  // inicializa o interior
  i = 1;
  while (i < n_x-1) {
    double x = a_x + i * h;
    j = 1;
    while (j < n_y-1) {
      double y = a_y + j * h;
      matrix_setAt(u, i, j, interior(x, y));
      j++;
    }
    i++;
  }

  return de_seidelElliptic(u, f, g, a_x, a_y, h, n_x, n_y, itermax, tol);
}

double de_ellipticMaxError(const Matrix* result, Real2Function u, double a_x, double a_y, double h) {
  int n_x = result->rows;
  int n_t = result->columns;
  
  double maxError = 0;

  int j = 0;
  while (j < n_t) {
    int i = 0;
    while (i < n_x) {
      double r_ij = matrix_at(result, i, j);
      double error = fabs(r_ij - u(a_x + i*h, a_y + j*h));

      if (error > maxError) {
        maxError = error;
      }
      i++;
    }
    j++;
  }
  return maxError;
}

#endif
