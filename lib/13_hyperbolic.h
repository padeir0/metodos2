#include "matrix.h"
#include "basicTypes.h"

#include <float.h>

#ifndef M2_hyperbolic_H
#define M2_hyperbolic_H

/*
Soluciona uma EDP no formato:
    u_{xx}(x,t) = u_{tt}(x, t)
    u_{t}(x, 0) = 0
    u(0, t) = 0
    u(1, t) = 0
    u(x, 0) = f(x)

Retorna uma matriz $n_x X n_t$, onde cada entrada $a_{ij}$
representa os valores de u(x_i, t_j), onde:
$$
  x_i = i*h
  t_j = j*k
$$

`n_x` é o número de nós no espaço (linhas), incluindo as duas
fronteiras x_0 = 0 e x_{n_x-1} = 0; logo há n_x-1 subintervalos.
`n_t` é o número de instantes (colunas), incluindo t_0 = 0; logo
são n_t-1 passos de tempo.

O tempo começa do zero, enquanto o espaço fica contido no intervalo [0, 1].
*/
Matrix* de_solveNaiveHyperbolic(RealFunction f, double h, double k, int n_t) {
  const double rho = (k*k)/(h*h);

  double length = 1 - 0;
  int n_x = (int)lround(length / h) + 1;
  Matrix* result = matrix_new(n_x, n_t);
  if (result == NULL) {
    return NULL;
  }

  // condições iniciais:
  {
    // w_i
    matrix_setAt(result, 0, 0, 0);
    matrix_setAt(result, n_x-1, 0, 0);
    // v_i
    matrix_setAt(result, 0, 1, 0);
    matrix_setAt(result, n_x-1, 1, 0);

    int i = 1;
    while (i < n_x-1) {
      double x = i * h;
      double w_i = f(x);
      double v_i = (rho/2)*(f(x-h) + f(x+h)) + (1-rho)*f(x);
      matrix_setAt(result, i, 0, w_i);
      matrix_setAt(result, i, 1, v_i);
      i++;
    }
  }

  int j = 2;
  while (j < n_t) {
    // fronteira
    matrix_setAt(result, 0, j, 0);
    matrix_setAt(result, n_x-1, j, 0);

    int i = 1;
    while (i < n_x-1) {
      double v_left  = matrix_at(result, i-1, j-1);
      double v       = matrix_at(result, i,   j-1);
      double v_right = matrix_at(result, i+1, j-1);
      double w       = matrix_at(result, i,   j-2);
      double out = rho * (v_left + v_right) + 2 * (1 - rho) * v - w;

      matrix_setAt(result, i, j, out);
      i++;
    }
    j++;
  }
  return result;
}

double de_hyperbolicMaxError(const Matrix* result, Real2Function u,
                            double h, double k) {
  int n_x = result->rows;
  int n_t = result->columns;
  
  double maxError = 0;

  int j = 0;
  while (j < n_t) {
    int i = 0;
    while (i < n_x) {
      double r_ij = matrix_at(result, i, j);
      double error = fabs(r_ij - u(i*h, j*k));

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
