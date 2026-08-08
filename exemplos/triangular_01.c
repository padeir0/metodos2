#include "../lib/matrix.h"
#include "../lib/01_subst.h"

#include <stdio.h>
#include <stdlib.h>

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

int main(void) {
  Matrix* A = matrix_new(3, 3);
  matrix_setIdentity(A);

  matrix_setAt(A, 0, 1, 1.0);
  matrix_setAt(A, 0, 2, 1.0);
  matrix_setAt(A, 1, 2, 1.0);

  printf("matriz A:\n");
  matrix_print(A, 2);
  bool upper = matrix_isUpperTriangular(A, 0);
  bool lower = matrix_isLowerTriangular(A, 0);
  printf("triangular superior = %s, triangular inferior = %s\n", upper? "true":"false", lower? "true":"false");

  printf("transposta de A:\n");
  matrix_transpose(A, A);
  matrix_print(A, 2);
  upper = matrix_isUpperTriangular(A, 0);
  lower = matrix_isLowerTriangular(A, 0);
  printf("triangular superior = %s, triangular inferior = %s\n", upper? "true":"false", lower? "true":"false");
  
  matrix_free(&A);
  return 0;
}
