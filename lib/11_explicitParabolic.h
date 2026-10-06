#include "matrix.h"
#include "basicTypes.h"
#include "04_tridiagonal.h"

#include <float.h>

#ifndef M2_explicitParabolic_H
#define M2_explicitParabolic_H

/*
Soluciona uma EDP no formato:
    u_{xx}(x,t) = u_t(x, t)
    u(a, t) = f(t)
    u(b, t) = g(t)
    u(x, 0) = w(x)

Retorna uma matriz $n_x X n_t$, onde cada entrada $a_{ij}$
representa os valores de u(x_i, t_j), onde:
$$
    h = (b-a)/(n_x-1)
    x_i = a + i*h
    t_j = 0 + j*(1/2)*(h^2)
$$

`n_x` é o número de nós no espaço (linhas), incluindo as duas
fronteiras x_0 = a e x_{n_x-1} = b; logo há n_x-1 subintervalos.
`n_t` é o número de instantes (colunas), incluindo t_0 = 0; logo
são n_t-1 passos de tempo.

O tempo começa do zero, enquanto o espaço fica contido no intervalo [a, b]
*/
Matrix* de_solveParabolic(RealFunction f, RealFunction g, RealFunction w,
                          double a, double b, int n_x, int n_t) {
  double h = (b-a)/(double)(n_x-1);
  double k = (h*h)/2;
  const double sigma = 0.5; // k/(h*h);

  Matrix* result = matrix_new(n_x, n_t);
  if (result == NULL) {
    return NULL;
  }

  // condições iniciais:
  {
    matrix_setAt(result, 0, 0, f(0));
    matrix_setAt(result, n_x-1, 0, g(0));
    int i = 1;
    while (i < n_x-1) {
      double x = a + i * h;
      matrix_setAt(result, i, 0, w(x));
      i++;
    }
  }

  int j = 1;
  while (j < n_t) {
    double t = j * k;
    // fronteira depende do tempo
    matrix_setAt(result, 0, j, f(t));
    matrix_setAt(result, n_x-1, j, g(t));

    int i = 1;
    while (i < n_x-1) {
      double u_left  = matrix_at(result, i-1, j-1);
      double u       = matrix_at(result, i,   j-1);
      double u_right = matrix_at(result, i+1, j-1);
      double out = sigma * ( u_left + u_right) + (1 - 2*sigma) * u;

      matrix_setAt(result, i, j, out);
      i++;
    }
    j++;
  }
  return result;
}

/*
Mesma EDP e mesmo esquema de `de_solveParabolic` mas usa um double buffer
ao invés de popular uma matriz $n_x x n_t$. Usa apenas 2 vetores $n_x x 1$
de memória.

A função MatrixMuncher recebe um desses vetores coluna e é chamada exatamente $n_t$
vezes. O vetor só pode ser usado durante o tempo de vida da chamada e é reutilizado
pela função em cada iteração.

Retorna false se faltar memória (nesse caso `munch` não é chamada).
Os resultados são idênticos, bit a bit, aos de `de_solveParabolic`.
*/
bool de_solveParabolicInPlace(RealFunction f, RealFunction g, RealFunction w,
                              double a, double b, int n_x, int n_t,
                              MatrixMuncher munch) {
  #if DEBUG
    assert(munch != NULL);
    assert(n_x >= 2);
    assert(n_t >= 1);
  #endif
  double h = (b-a)/(double)(n_x-1);
  double k = (h*h)/2;
  const double sigma = 0.5; // k/(h*h);

  Matrix* current = matrix_new(n_x, 1);
  Matrix* next = matrix_new(n_x, 1);
  if (current == NULL || next == NULL) {
    matrix_free(&current);
    matrix_free(&next);
    return false;
  }

  // condições iniciais:
  {
    matrix_setAt(current, 0, 0, f(0));
    matrix_setAt(current, n_x-1, 0, g(0));
    int i = 1;
    while (i < n_x-1) {
      double x = a + i * h;
      matrix_setAt(current, i, 0, w(x));
      i++;
    }
  }
  munch(current);

  int j = 1;
  while (j < n_t) {
    double t = j * k;
    // fronteira depende do tempo
    matrix_setAt(next, 0, 0, f(t));
    matrix_setAt(next, n_x-1, 0, g(t));

    int i = 1;
    while (i < n_x-1) {
      double u_left  = matrix_at(current, i-1, 0);
      double u       = matrix_at(current, i,   0);
      double u_right = matrix_at(current, i+1, 0);
      double out = sigma * ( u_left + u_right) + (1 - 2*sigma) * u;

      matrix_setAt(next, i, 0, out);
      i++;
    }
    munch(next);

    Matrix* tmp = current;
    current = next;
    next = tmp;
    j++;
  }

  matrix_free(&current);
  matrix_free(&next);
  return true;
}

/* Recebe uma solução exata, achada analiticamente, e calcula
o erro do resultado em comparação com essa solução.
*/
double de_parabolicMaxError(const Matrix* result, Real2Function u,
                            double a, double b) {
  int n_x = result->rows;
  int n_t = result->columns;
  
  double h = (b-a)/(double)(n_x-1);
  double k = (h*h)/2;

  double maxError = 0;

  int j = 0;
  while (j < n_t) {
    int i = 0;
    while (i < n_x) {
      double r_ij = matrix_at(result, i, j);
      double error = fabs(r_ij - u(a + i*h, j*k));

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
