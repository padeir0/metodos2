
#include "../lib/matrix.h"
#include "../lib/11_explicitParabolic.h"

#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979323846

#define X_A 0.0
#define X_B 1.0
#define N_X 11
#define N_T 21

double f(double t) { return exp(-t) + t; }
double g(double t) { return exp(-t) * cos(1.0) + t + 0.5; }
double w(double x) { return cos(x) + 0.5 * x * x; }
double exact(double x, double t) { return exp(-t) * cos(x) + t + 0.5 * x * x; }

int main(void) {
  Matrix* U = de_solveParabolic(f, g, w, X_A, X_B, N_X, N_T);
  if (U == NULL) {
    fprintf(stderr, "falha ao alocar a solução\n");
    return 1;
  }

  printf("# x t u\n");
  int j = 0;
  while (j < N_T) {
    int i = 0;
    while (i < N_X) {
      printf("%.4f\t", matrix_at(U, i, j));
      i++;
    }
    printf("\n");
    j++;
  }

  printf("# erro maximo = %.6e\n", de_parabolicMaxError(U, exact, X_A, X_B));

  matrix_free(&U);
  return 0;
}
