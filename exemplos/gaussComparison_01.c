#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/02_gauss.h"
#include "../lib/03_gaussPivot.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR 1e-16
#define ORDER 4

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

double A_data[ORDER*ORDER] = {
  1e-12,   1,   1,   1,
    1, 1e-12,   1,   1,
    1,   1, 1e-12,   1,
    1,   1,   1, 1e-12,
};
double X_data[ORDER] = {1,1,1,1};

int main(void) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* X = matrix_new(ORDER, 1);
  Matrix* B = matrix_new(ORDER, 1);
  bool ok;

  /* criamos um sistema com solução conhecida: X = {1,1,1,1} */
  matrix_set(A_data, ORDER, ORDER, A);
  matrix_set(X_data, ORDER, 1, X);
  matrix_mult(A, X, B);

  Matrix* A_gauss = matrix_newCopy(A);
  Matrix* B_gauss = matrix_newCopy(B);
  Matrix* X_gauss = matrix_new(ORDER, 1);

  Matrix* A_gaussPivot = matrix_newCopy(A);
  Matrix* B_gaussPivot = matrix_newCopy(B);
  Matrix* X_gaussPivot = matrix_new(ORDER, 1);

  /* eliminação Gaussiana ingênua */

  ok = matrix_sqGaussianElimination(A_gauss, B_gauss, ERROR);
  if (!ok) {
    printf("Eliminação gaussiana falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  ok = matrix_solveBySubstitution(A_gauss, X_gauss, B_gauss, ERROR);
  if (!ok) {
    printf("Não deu pra solucionar a matriz! Certeza que ela é triangular?\n");
    abort();
  }

  /* eliminação Gaussiana com pivotamento */
  ok = matrix_sqGaussianEliminationPivot_2(A_gaussPivot, B_gaussPivot, ERROR);
  if (!ok) {
    printf("Eliminação gaussiana c/ pivotamento falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  ok = matrix_solveBySubstitution(A_gaussPivot, X_gaussPivot, B_gaussPivot, ERROR);
  if (!ok) {
    printf("Não deu pra solucionar a matriz! Certeza que ela é triangular?\n");
    abort();
  }

  printf("solução exata:\n");
  matrix_print(X, 12);
  printf("solução por gauss:\n");
  matrix_print(X_gauss, 12);
  printf("solução por gauss c/ pivotamento:\n");
  matrix_print(X_gaussPivot, 12);

  matrix_free(&A);
  matrix_free(&X);
  matrix_free(&B);
  matrix_free(&A_gauss);
  matrix_free(&X_gauss);
  matrix_free(&B_gauss);
  matrix_free(&A_gaussPivot);
  matrix_free(&X_gaussPivot);
  matrix_free(&B_gaussPivot);
  return 0;
}
