/* PVC linear da aula 11:

     x'' = e^t - 3 sin t + x' - x,   x(1) = e - 3 cos 1,   x(2) = e^2 - 3 cos 2

   isto é, x'' = u(t) + v(t) x + w(t) x' com u = e^t - 3 sin t, v = -1, w = 1.
   Solução exata: x(t) = e^t - 3 cos t.

   Reproduz o que o slide pede: n = 99 subintervalos (h = 1/99, 98 incógnitas),
   imprimindo t, solução e erro (exata - numérica) nos extremos e a cada 9 nós
   (nós 9, 18, ..., 90, que é o que a tabela do slide mostra).

   NOTE(1): a tabela do slide foi calculada em precisão simples. Em `double` o
   erro máximo é ~1.3e-6, e não ~4e-4 como no slide.

   Convenção de `matrix_buildTridiagonalSystem`: `n` é o número de subintervalos,
   as matrizes têm n-1 linhas e a linha `i` (base 0) corresponde ao nó
   t_{i+1} = a + (i+1) h.
*/

#include "../lib/matrix.h"
#include "../lib/04_tridiagonal.h"
#include "../lib/10_linearDiffEq.h"

#include <math.h>
#include <stdio.h>

#define T_A 1.0
#define T_B 2.0
#define N 99
#define PRINT_STEP 9

double odeU(double t) { return exp(t) - 3.0 * sin(t); }
double odeV(double t) { (void)t; return -1.0; }
double odeW(double t) { (void)t; return 1.0; }
double exact(double t) { return exp(t) - 3.0 * cos(t); }

int main(void) {
  int rows = N - 1;
  double h = (T_B - T_A) / N;
  double alpha = exact(T_A);
  double beta = exact(T_B);

  Matrix* A = matrix_new(rows, 1);
  Matrix* B = matrix_new(rows, 1);
  Matrix* C = matrix_new(rows, 1);
  Matrix* D = matrix_new(rows, 1);
  Matrix* X = matrix_new(rows, 1);

  matrix_buildTridiagonalSystem(odeU, odeV, odeW, A, B, C, D, N,
                                T_A, T_B, alpha, beta);
  matrix_solveTridiagonal(A, B, C, X, D);

  printf("%4s %12s %14s %12s\n", "i", "t", "solucao", "erro");
  printf("%4d %12.7f %14.7f %12.2e\n", 0, T_A, alpha, exact(T_A) - alpha);

  double maxError = 0.0;
  int i = 0;
  while (i < rows) {
    int node = i + 1;
    double t = T_A + node * h;
    double error = exact(t) - matrix_at(X, i, 0);
    /* escrito assim para que NaN propague em vez de ser ignorado */
    if (!(fabs(error) <= maxError)) {
      maxError = fabs(error);
    }
    if (node % PRINT_STEP == 0) {
      printf("%4d %12.7f %14.7f %12.2e\n", node, t, matrix_at(X, i, 0), error);
    }
    i++;
  }

  printf("%4d %12.7f %14.7f %12.2e\n", N, T_B, beta, exact(T_B) - beta);
  printf("\nerro maximo (todos os nos) = %.3e\n", maxError);

  matrix_free(&A);
  matrix_free(&B);
  matrix_free(&C);
  matrix_free(&D);
  matrix_free(&X);
  return 0;
}
