#include "../lib/matrix.h"
#include "../lib/06_lu.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR 1e-9
#define ORDER 4

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

double A_data[ORDER*ORDER] = {
  1.0,  3.0, 2.0, 1,
    0,  3.0,   1, 1,
    0,    0,   1, 0,
   -1,   10,   0, 4,
};

int main(void) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* P = matrix_new(ORDER, ORDER);
  Matrix* L = matrix_new(ORDER, ORDER);
  Matrix* U = matrix_new(ORDER, ORDER);
  bool ok;

  matrix_set(A_data, ORDER, ORDER, A);

  printf("matriz A:\n");
  matrix_print(A, 2);

  ok = matrix_NaiveLUDecomposition(A, L, U, ERROR);
  if (!ok) {
    printf("Decomposição LU ingênua falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  // verifica se A = LU
  {
    Matrix* LU = matrix_new(ORDER, ORDER);
    matrix_mult(L, U, LU);
    printf("matriz LU:\n");
    matrix_print(LU, 2);

    if (matrix_equals(A, LU, ERROR)) {
      printf("Decomposição LU ingênua: OK!\n");
    } else {
      printf("Decomposição LU ingênua: FALHOU!\n");
    }
    matrix_free(&LU);
  }

  ok = matrix_PivotingLUDecomposition(A, P, L, U, ERROR);
  if (!ok) {
    printf("Decomposição LU c/ pivotamento falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  // verifica se PA = LU
  {
    Matrix* PA = matrix_new(ORDER, ORDER);
    Matrix* LU = matrix_new(ORDER, ORDER);
    matrix_mult(P, A, PA);
    matrix_mult(L, U, LU);
    printf("matriz PA:\n");
    matrix_print(PA, 2);
    printf("matriz LU:\n");
    matrix_print(LU, 2);

    if (matrix_equals(PA, LU, ERROR)) {
      printf("Decomposição LU c/ pivotamento: OK!\n");
    } else {
      printf("Decomposição LU c/ pivotamento: FALHOU!\n");
    }
    matrix_free(&PA);
    matrix_free(&LU);
  }

  matrix_free(&P);
  matrix_free(&A);
  matrix_free(&L);
  matrix_free(&U);
  return 0;
}
