#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/02_gauss.h"
#include "../lib/03_gaussPivot.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERROR 1e-16

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

/* cria um sistema linear nas matrizes dadas com solução conhecida */
void createLinearSystem(Matrix* A, Matrix* X, Matrix* B) {
  #if DEBUG
    /* garante que o sistema tem formato correto */
    assert(A->rows == A->columns);
    assert(A->columns == X->rows);
    assert(B->rows == X->rows);
  #endif
  int i;
  int j;

  i = 0;
  while (i < A->rows) {
    j = 0;
    while (j < A->columns) {
      if (i != j) {
        matrix_setAt(A, i, j, 1);
      }
      j++;
    }
    matrix_setAt(A, i, i, 1e-12);
    i++;
  }

  // matriz X é {1.0, ..., 1.0}
  i = 0;
  while (i < X->rows) {
    matrix_setAt(X, i, 0, 1.0);
    i++;
  }

  // matriz B é determinada a partir de A*X
  matrix_mult(A, X, B);
}

void performComparison(int ORDER) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* X = matrix_new(ORDER, 1);
  Matrix* B = matrix_new(ORDER, 1);
  bool ok;

  createLinearSystem(A, X, B);

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
