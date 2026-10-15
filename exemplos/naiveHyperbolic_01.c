#include "../lib/matrix.h"
#include "../lib/13_hyperbolic.h"

#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979323846

#define X_A 0.0
#define X_B 1.0
#define N_T 100

double initial(double x) { return sin(PI * x); }
double exact(double x, double t) { return sin(PI * x) * cos(PI *t); }

int main(void) {
  const double h = 0.05;
  const double k = 0.05;
  Matrix* U = de_solveNaiveHyperbolic(initial, h, k, N_T);
  if (U == NULL) {
    fprintf(stderr, "falha ao alocar a solução\n");
    return 1;
  }

  printf("# x t u\n");
  int j = 0;
  while (j < U->columns) {
    int i = 0;
    while (i < U->rows) {
      printf("%.4f\t", matrix_at(U, i, j));
      i++;
    }
    printf("\n");
    j++;
  }

  printf("# erro maximo = %.6e\n", de_hyperbolicMaxError(U, exact, h, k));

  matrix_free(&U);
  return 0;
}
