#include "../lib/matrix.h"
#include "../lib/07_jacobiGaussSeidel.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// número máximo de iterações do algoritmo
#define MAXITER 1000

int solveForN(int order, double tol) {
  int prec = (int)fabs(ceil(log10(tol)));

  Matrix* A = matrix_new(order, order);
  Matrix* X_jacobi = matrix_new(order, 1);
  Matrix* B = matrix_new(order, 1);

  Matrix* solution = matrix_new(order, 1);
  matrix_setAll(solution, 1); // solution = [1, ..., 1]

  matrix_createLinearSystem(A, solution, B, 1e-4, 1e4);
  matrix_setAll(X_jacobi, 2);

  int iter_jacobi = matrix_jacobi(A, B, X_jacobi, MAXITER, tol);
  if (iter_jacobi == MAXITER) {
    printf("Método de Jacobi não convergiu!\n");
    printf("matriz X:\n");
    matrix_print(X_jacobi, prec);
    abort();
  }

  bool solved_jacobi = matrix_equals(solution, X_jacobi, tol);
  if (solved_jacobi == false) {
    printf("Falhou! Tol: %.*f:\n", prec, tol);
    matrix_print(X_jacobi, prec);
  }

  matrix_free(&A);
  matrix_free(&X_jacobi);
  matrix_free(&B);
  matrix_free(&solution);
  return iter_jacobi;
}

int main(void) {
  srand(42);
  double tol = 1;
  printf("# tol\t\titer\n");
  while (tol > 1e-12) {
    int i = 0;
    double sum = 0;
    while (i < 100) {
      sum += (double)solveForN(8, tol);
      i++;
    }
    int prec = (int)fabs(ceil(log10(tol)));
    printf("%d, %.1f\n", prec, sum/(double)i);
    tol /= 10;
  }
}
