#include "../lib/matrix.h"
#include "../lib/11_explicitParabolic.h"

#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979323846

#define X_A 0.0
#define X_B 1.0
#define T_FINAL 0.5

#define ORDER 2.0
#define ORDER_TOLERANCE 0.1
#define N_START 10
#define N_MAX 160

double boundary(double t) { (void)t; return 0.0; }
double initial(double x) { return sin(PI * x); }
double exact(double x, double t) { return exp(-PI * PI * t) * sin(PI * x); }

/* resolve com `n` subintervalos, até T_FINAL, e devolve o erro máximo
   |exata - numérica| em toda a grade; devolve NaN se faltar memória */
double maxError(int n) {
  double h = (X_B - X_A) / n;
  double k = (h * h) / 2;
  int steps = (int)floor(T_FINAL / k + 0.5);

  Matrix* U = de_solveParabolic(boundary, boundary, initial,
                                X_A, X_B, n + 1, steps + 1);
  if (U == NULL) {
    return NAN;
  }
  double error = de_parabolicMaxError(U, exact, X_A, X_B);
  matrix_free(&U);
  return error;
}

int main(void) {
  bool ok = true;
  int comparisons = 0;
  double previousError = 0.0;

  printf("%6s %10s %9s %14s %10s %10s\n",
         "n", "h", "n_t", "erro maximo", "razao", "ordem");

  int n = N_START;
  while (n <= N_MAX) {
    double h = (X_B - X_A) / n;
    int n_t = (int)floor(T_FINAL / ((h * h) / 2) + 0.5) + 1;
    double error = maxError(n);

    if (n == N_START) {
      printf("%6d %10.6f %9d %14.6e %10s %10s\n", n, h, n_t, error, "-", "-");
    } else {
      double ratio = previousError / error;
      double order = log2(ratio);
      /* `<` é falso para NaN e infinito: ambos reprovam */
      bool pass = fabs(order - ORDER) < ORDER_TOLERANCE;
      printf("%6d %10.6f %9d %14.6e %10.4f %10.4f%s\n",
             n, h, n_t, error, ratio, order,
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
