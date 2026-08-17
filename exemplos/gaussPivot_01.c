#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/03_gaussPivot.h"

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
  1.0, 3.0, 2.0, 1,
    0, 3.0,   1, 1,
    0,   0,   1, 0,
   -1,   0,   0, 4,
};
double B_data[ORDER] = {1.0, 2.0, 0, 3.0};

int main(void) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* X = matrix_new(ORDER, 1);
  Matrix* B = matrix_new(ORDER, 1);

  matrix_set(A_data, ORDER, ORDER, A);
  matrix_set(B_data, ORDER, 1, B);

  Matrix* A_copy = matrix_newCopy(A);
  Matrix* B_copy = matrix_newCopy(B);

  printf("matriz A:\n");
  matrix_print(A, 2);
  printf("vetor B:\n");
  matrix_print(B, 2);

  bool ok = matrix_sqGaussianEliminationPivot_2(A, B, ERROR);
  if (!ok) {
    printf("Eliminação gaussiana falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  printf("\nELIMINAÇÃO GAUSSIANA!\n");

  printf("matriz A:\n");
  matrix_print(A, 2);
  printf("vetor B:\n");
  matrix_print(B, 2);

  ok = matrix_solveBySubstitution(A, X, B, ERROR);
  if (!ok) {
    printf("Não deu pra solucionar a matriz! Certeza que ela é triangular?\n");
    abort();
  }

  printf("matriz X:\n");
  matrix_print(X, 2);

  {
    printf("matrix_mult:\n");
    Matrix* result = matrix_new(B->rows, B->columns);
    matrix_mult(A, X, result);
    matrix_print(result, 2);
    matrix_free(&result);
  }

  /* Verifica se a solução resolve o problema original, antes da eliminação
     gaussiana. */
  bool solved = matrix_verifySolution(A_copy, X, B_copy, ERROR);
  if (solved) {
    printf("Solução correta!\n");
  } else {
    printf("Solução incorreta!\n");
  }

  matrix_free(&A);
  matrix_free(&X);
  matrix_free(&B);
  matrix_free(&A_copy);
  matrix_free(&B_copy);
  return 0;
}
