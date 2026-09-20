#include "../lib/matrix.h"
#include "../lib/09_gradiente.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// número máximo de iterações do algoritmo
// 2^12
#define MAXITER (1<<10)

typedef struct {
  int iter;
  clock_t time;
} stats;

stats solveForN(int order, double tol) {
  stats out;
  int prec = (int)fabs(ceil(log10(tol)));

  Matrix* A = matrix_new(order, order);
  Matrix* X_grad = matrix_new(order, 1);
  Matrix* B = matrix_new(order, 1);

  Matrix* solution = matrix_new(order, 1);
  matrix_setAll(solution, 1); // solution = [1, ..., 1]

  matrix_createLinearSystem(A, solution, B, 1e-4, 1e4);
  matrix_setAll(X_grad, 0);

  clock_t start = clock();
  int iter_grad = matrix_gradientSteepestDescent(A, X_grad, B, MAXITER, tol);
  if (iter_grad >= MAXITER) {
    printf("Método dos Gradientes não convergiu!\n");
    double dist = matrix_distanceSquared(X_grad, solution);
    printf("tol²: %.*f, distance²: %.*f\n", prec*2, tol*tol, prec*2, dist);
    printf("matriz X:\n");
    matrix_print(X_grad, prec+2);
    abort();
  }
  clock_t end = clock();
  out.time = end-start;

  bool solved_jacobi = matrix_equals(solution, X_grad, tol);
  if (solved_jacobi == false) {
    double dist = sqrt(matrix_distanceSquared(X_grad, solution));
    printf("Falhou! Tol: %.*f, distance: %.*f:\n", prec+2, tol, prec+2, dist);
    matrix_print(X_grad, prec);
  }

  matrix_free(&A);
  matrix_free(&X_grad);
  matrix_free(&B);
  matrix_free(&solution);
  out.iter = iter_grad;
  return out;
}

int main(void) {
  srand(42);
  double tol = 1;
  printf("# tol\titer\tseconds\n");
  while (tol > 1e-8) {
    int i = 0;
    double iter_sum = 0;
    double seconds_sum = 0;
    while (i < 100) {
      stats out = solveForN(8, tol);
      iter_sum += (double)out.iter;
      seconds_sum += (double)out.time / (double)CLOCKS_PER_SEC;
      i++;
    }
    int prec = (int)fabs(ceil(log10(tol)));
    iter_sum /= (double)i;
    seconds_sum /= (double)i;
    printf("%d, %.1f, %.6f\n", prec, iter_sum, seconds_sum);
    tol /= 10;
  }
}
