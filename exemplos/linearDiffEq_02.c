/* Ordem de convergência do PVC linear da aula 11:

     x'' = e^t - 3 sin t + x' - x,   x(1) = e - 3 cos 1,   x(2) = e^2 - 3 cos 2

   Solução exata: x(t) = e^t - 3 cos t.

   Para um esquema de ordem p, erro(n) ~ C h^p com h = (b-a)/n. Dobrar n
   (h -> h/2) divide o erro por 2^p, logo a ordem observada é

     p = log2( erro(n) / erro(2n) )

   Diferenças centrais para x' e x'' têm p = 2. O exemplo começa em N_START,
   dobra n até N_MAX e, a cada passo, exige |p - ORDER| < ORDER_TOLERANCE.
   Sai com código 1 se algum passo falhar.

   NOTE(1): ORDER_TOLERANCE = 0.1 é uma margem escolhida, não deduzida. O pior
   desvio medido na faixa usada é 0.009 (n = 11 -> 22, ainda pré-assintótico);
   depois cai para < 1e-3. Para diferenças centrais espera-se que o erro tenha
   só potências pares de h, e portanto que o desvio decaia como h^2.

   NOTE(2): N_MAX limita o teste porque, em `double`, o arredondamento cresce
   com o número de condição (~ n^2) e passa a dominar o erro de truncação. Medido
   com este mesmo código: n = 704 -> desvio -0.0008; n = 1408 -> +0.042;
   n = 2816 -> razão 2.65 (p = 1.4). Acima de ~1e3 o teste deixa de medir a ordem.

   NOTE(3): em `float` este teste não vale: já em n = 99 o erro é ~4e-4, quase
   todo arredondamento (veja a tabela do slide, NOTE(1) de linearDiffEq_01.c).

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

#define ORDER 2.0
#define ORDER_TOLERANCE 0.1
#define N_START 11
#define N_MAX 1000

double odeU(double t) { return exp(t) - 3.0 * sin(t); }
double odeV(double t) { (void)t; return -1.0; }
double odeW(double t) { (void)t; return 1.0; }
double exact(double t) { return exp(t) - 3.0 * cos(t); }

/* resolve o PVC com `n` subintervalos e devolve o erro máximo
   |exata - numérica| nos nós internos */
double maxError(int n) {
  int rows = n - 1;
  double h = (T_B - T_A) / n;

  Matrix* A = matrix_new(rows, 1);
  Matrix* B = matrix_new(rows, 1);
  Matrix* C = matrix_new(rows, 1);
  Matrix* D = matrix_new(rows, 1);
  Matrix* X = matrix_new(rows, 1);

  de_LinearBuildTriSys(odeU, odeV, odeW, A, B, C, D, n,
                                T_A, T_B, exact(T_A), exact(T_B));
  matrix_solveTridiagonal(A, B, C, X, D);

  double result = 0.0;
  int i = 0;
  while (i < rows) {
    double t = T_A + (i + 1) * h;
    double error = fabs(exact(t) - matrix_at(X, i, 0));
    /* escrito assim para que NaN propague em vez de ser ignorado */
    if (!(error <= result)) {
      result = error;
    }
    i++;
  }

  matrix_free(&A);
  matrix_free(&B);
  matrix_free(&C);
  matrix_free(&D);
  matrix_free(&X);
  return result;
}

int main(void) {
  bool ok = true;
  int comparisons = 0;
  double previousError = 0.0;

  printf("%7s %14s %10s %10s\n", "n", "erro maximo", "razao", "ordem");

  int n = N_START;
  while (n <= N_MAX) {
    double error = maxError(n);

    if (n == N_START) {
      printf("%7d %14.6e %10s %10s\n", n, error, "-", "-");
    } else {
      double ratio = previousError / error;
      double order = log2(ratio);
      /* `<` é falso para NaN e infinito: ambos reprovam */
      bool pass = fabs(order - ORDER) < ORDER_TOLERANCE;
      printf("%7d %14.6e %10.4f %10.4f%s\n", n, error, ratio, order,
             pass ? "" : "   <-- fora da tolerancia");
      if (!pass) {
        ok = false;
      }
      comparisons++;
    }

    previousError = error;
    n = 2 * n;
  }

  /* sem nenhuma comparação o teste não verificou nada */
  if (comparisons == 0) {
    ok = false;
  }

  if (ok) {
    printf("\nordem observada = %.1f +- %.1f em todos os passos: solução correta!\n",
           ORDER, ORDER_TOLERANCE);
    return 0;
  }
  printf("\nsolução incorreta!\n");
  return 1;
}
