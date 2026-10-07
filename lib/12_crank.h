#include "matrix.h"
#include "basicTypes.h"
#include "04_tridiagonal.h"

#include <float.h>

#ifndef M2_crank_H
#define M2_crank_H

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

(Esse método não é o método de crank-nicolson de verdade, na verdade é
 o backward-euler enrustido, o erro desse solver é pior que o da solução
 explicita [11_explicitParabolic.h] por 1 ordem de magnitude)
*/
Matrix* de_badCrank(RealFunction f, RealFunction g, RealFunction w,
                 double a, double b, int n_x, int n_t) {
  double h = (b-a)/(double)(n_x-1);
  double k = (h*h)/2;
  const double s = (h*h)/k;   // = 2
  const double r = 2 + s;     // = 4
  const int m = n_x - 2;      // número de incógnitas internas (precisa de n_x >= 3)

  Matrix* result = matrix_new(n_x, n_t);
  if (result == NULL) {
    return NULL;
  }

  Matrix* A = matrix_new(m, 1);
  Matrix* B = matrix_new(m, 1);
  Matrix* C = matrix_new(m, 1);
  Matrix* X = matrix_new(m, 1);
  Matrix* D = matrix_new(m, 1);

  if (A == NULL || B == NULL || C == NULL || X == NULL || D == NULL) {
    matrix_free(&A); matrix_free(&B); matrix_free(&C);
    matrix_free(&X); matrix_free(&D);
    matrix_free(&result);
    return NULL;
  }

  // A e C não são modificados pelo solver
  matrix_setAll(A, -1);
  matrix_setAll(C, -1);

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
    double f_t = f(t);
    double g_t = g(t);

    // fronteira depende do tempo
    matrix_setAt(result, 0, j, f_t);
    matrix_setAt(result, n_x-1, j, g_t);

    // B e D são corrompidos pelo solver: refazer a cada passo.
    // A incógnita p do sistema corresponde ao nó i = p+1.
    matrix_setAll(B, r);
    int p = 0;
    while (p < m) {
      double d_p = s * matrix_at(result, p+1, j-1);
      matrix_setAt(D, p, 0, d_p);
      p++;
    }

    // Os valores de fronteira conhecidos vão para o lado direito
    // (o termo -u_0 da primeira equação passa somando para o outro lado)
    matrix_setAt(D, 0,   0, matrix_at(D, 0,   0) + f_t);
    matrix_setAt(D, m-1, 0, matrix_at(D, m-1, 0) + g_t);

    // corrompe B e D, o resultado sai em X
    matrix_solveTridiagonal(A, B, C, X, D);

    p = 0;
    while (p < m) {
      matrix_setAt(result, p+1, j, matrix_at(X, p, 0));
      p++;
    }

    j++;
  }

  matrix_free(&A);
  matrix_free(&B);
  matrix_free(&C);
  matrix_free(&D);
  matrix_free(&X);
  return result;
}

Matrix* de_crank(RealFunction f, RealFunction g, RealFunction w,
                 double a, double b, int n_x, int n_t, double k) {
  double h = (b-a)/(double)(n_x-1);
  double sigma = k/(h*h);

  Matrix* result = matrix_new(n_x, n_t);
  if (result == NULL) return NULL;

  Matrix* A = matrix_new(n_x, 1);
  Matrix* B = matrix_new(n_x, 1);
  Matrix* C = matrix_new(n_x, 1);
  Matrix* X = matrix_new(n_x, 1);
  Matrix* D = matrix_new(n_x, 1);
  if (A == NULL || B == NULL || C == NULL || X == NULL || D == NULL) {
    matrix_free(&A); matrix_free(&B); matrix_free(&C);
    matrix_free(&X); matrix_free(&D);
    matrix_free(&result);
    return NULL;
  }

  // A e C não são modificados pelo solver, então basta montar uma vez
  matrix_setAll(A, -sigma/2);
  matrix_setAll(C, -sigma/2);
  matrix_setAt(C, 0, 0, 0);       // linha 0:     1*x_0 = f(t)
  matrix_setAt(A, n_x-1, 0, 0);   // linha n_x-1: 1*x_{n-1} = g(t)

  // condição inicial
  matrix_setAt(result, 0, 0, f(0));
  matrix_setAt(result, n_x-1, 0, g(0));
  for (int i = 1; i < n_x-1; i++) {
    matrix_setAt(result, i, 0, w(a + i*h));
  }

  for (int j = 1; j < n_t; j++) {
    double t = j * k;

    // B e D são corrompidos pelo solver: refazer a cada passo
    matrix_setAll(B, 1 + sigma);
    matrix_setAt(B, 0, 0, 1);
    matrix_setAt(B, n_x-1, 0, 1);

    matrix_setAt(D, 0, 0, f(t));
    matrix_setAt(D, n_x-1, 0, g(t));
    for (int i = 1; i < n_x-1; i++) {
      double u_left  = matrix_at(result, i-1, j-1);
      double u       = matrix_at(result, i,   j-1);
      double u_right = matrix_at(result, i+1, j-1);
      matrix_setAt(D, i, 0, (sigma/2)*(u_left + u_right) + (1 - sigma)*u);
    }

    matrix_solveTridiagonal(A, B, C, X, D);

    for (int i = 0; i < n_x; i++) {
      matrix_setAt(result, i, j, matrix_at(X, i, 0));
    }
  }

  matrix_free(&A); matrix_free(&B); matrix_free(&C);
  matrix_free(&D); matrix_free(&X);
  return result;
}

/* Recebe uma solução exata, achada analiticamente, e calcula
o erro do resultado em comparação com essa solução.
*/
double de_crankMaxError(const Matrix* result, Real2Function u,
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
