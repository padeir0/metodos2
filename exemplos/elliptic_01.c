#include "../lib/matrix.h"
#include "../lib/14_elliptic.h"

#include <math.h>
#include <stdio.h>

double solution(double x, double y) { return cosh(0.2 * x) + cosh(0.2 * y);}

double f(double x, double y) { (void)x; (void)y; return -0.04; }
double g(double x, double y) { (void)x; (void)y;  return 0; }
double boundary(double x, double y) { return solution(x, y);}
double start(double x, double y) { (void)x; (void)y;  return 1; }

int main(void) {
  const double a_x = 0;
  const double b_x = 1;
  const double a_y = 0;
  const double b_y = 1;
  const int n_x = 8;
  const int n_y = 8;
  const int itermax = 128;
  const double tol = 1e-9;

  Matrix* U;
  int iter = de_solveElliptic(&U, f, g, boundary, start, a_x, b_x, a_y, b_y, n_x, n_y, itermax, tol);
  if (U == NULL) {
    matrix_free(&U);
    fprintf(stderr, "falha ao alocar a solução\n");
    return 1;
  }
  if (iter == itermax) {
    fprintf(stderr, "algoritmo não convergiu\n");
    matrix_free(&U);
    return 1;
  }

  int i = 0;
  while (i < n_x) {
    int j = 0;
    while (j < n_y) {
      printf("%.4f\t", matrix_at(U, i, j));
      j++;
    }
    printf("\n");
    i++;
  }

  double h = de_ellipticIntervalWidth(a_x, b_x, n_x);
  printf("# erro maximo = %.6e\n", de_ellipticMaxError(U, solution, a_x, a_y, h));
  matrix_free(&U);
  return 0;
}
