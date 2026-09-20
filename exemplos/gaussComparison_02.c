#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/02_gauss.h"
#include "../lib/03_gaussPivot.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERROR 1e-12

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

void performComparison(int ORDER) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* X = matrix_new(ORDER, 1);
  Matrix* B = matrix_new(ORDER, 1);
  bool ok;

  matrix_setAll(X, 1); // solution = [1, ..., 1]
  matrix_createLinearSystem(A, X, B, 1e-2, 1e2);

  Matrix* A_gauss = matrix_newCopy(A);
  Matrix* B_gauss = matrix_newCopy(B);
  Matrix* X_gauss = matrix_new(ORDER, 1);

  Matrix* A_gaussPivot = matrix_newCopy(A);
  Matrix* B_gaussPivot = matrix_newCopy(B);
  Matrix* X_gaussPivot = matrix_new(ORDER, 1);

  Matrix* errorVector_gauss = matrix_newCopy(X);
  Matrix* errorVector_gaussPivot = matrix_newCopy(X);

  /* eliminação Gaussiana ingênua */
  clock_t start_gauss = clock();
  ok = matrix_sqGaussianElimination(A_gauss, B_gauss, ERROR);
  if (!ok) {
    printf("Eliminação gaussiana falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  clock_t end_gauss = clock();
  ok = matrix_solveBySubstitution(A_gauss, X_gauss, B_gauss, ERROR);
  if (!ok) {
    printf("Não deu pra solucionar a matriz! Certeza que ela é triangular?\n");
    abort();
  }

  /* eliminação Gaussiana com pivotamento */
  clock_t start_gaussPivot = clock();
  ok = matrix_sqGaussianEliminationPivot_2(A_gaussPivot, B_gaussPivot, ERROR);
  if (!ok) {
    printf("Eliminação gaussiana c/ pivotamento falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  clock_t end_gaussPivot = clock();
  ok = matrix_solveBySubstitution(A_gaussPivot, X_gaussPivot, B_gaussPivot, ERROR);
  if (!ok) {
    printf("Não deu pra solucionar a matriz! Certeza que ela é triangular?\n");
    abort();
  }

  double time_gauss = ((double) (end_gauss - start_gauss)) / CLOCKS_PER_SEC;
  double time_gaussPivot = ((double) (end_gaussPivot - start_gaussPivot)) / CLOCKS_PER_SEC;
  printf("%d, %f, %f\n", ORDER, time_gauss, time_gaussPivot);

  matrix_free(&A);
  matrix_free(&X);
  matrix_free(&B);
  matrix_free(&A_gauss);
  matrix_free(&X_gauss);
  matrix_free(&B_gauss);
  matrix_free(&A_gaussPivot);
  matrix_free(&X_gaussPivot);
  matrix_free(&B_gaussPivot);
  matrix_free(&errorVector_gauss);
  matrix_free(&errorVector_gaussPivot);
}

int main(void) {
  int order = 2;
  while (order < 2100) {
    performComparison(order);
    order *= 2;
  }
  return 0;
}
