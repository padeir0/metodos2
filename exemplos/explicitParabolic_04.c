#include "../lib/matrix.h"
#include "../lib/11_explicitParabolic.h"

#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979323846

#define X_A 0.0
#define X_B 1.0
#define N_X 11
#define N_T 21

double boundary(double t) { (void)t; return 0.0; }
double initial(double x) { return sin(PI * x); }
double exact(double x, double t) { return exp(-PI * PI * t) * sin(PI * x); }

void printRows(Matrix* m) {
  int i = 0;
  while (i < m->rows) {
    double u = matrix_at(m, i, 0);
    printf("%.4f\t", u);
    i++;
  }
  printf("\n");
}

int main(void) {
  printf("# x t u\n");

  bool ok = de_solveParabolicInPlace(boundary, boundary, initial,
                                     X_A, X_B, N_X, N_T, printRows);
  if (!ok) {
    fprintf(stderr, "falha ao alocar os buffers\n");
    return 1;
  }
  return 0;
}
