#include "../lib/matrix.h"
#include "../lib/05_gaussJordan.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR 1e-8

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

void setMatrixNGE(Matrix* A, Matrix* B) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(A->rows == A->columns);
    assert(B->rows == A->columns);
    assert(B->columns == 1);
  #endif
  int N = A->rows;
  int i = 0;
  while (i < N) {
    // o algoritmo original ta escrito em fortran (1 <= i <= n)
    double _i = (double)i+1;
    
    double b_i = (pow(_i+1, N) - 1) / (double)_i;
    matrix_setAt(B, i, 0, b_i);

    int j = 0;
    while (j < N) {
      double _j = (double)j+1;

      double a_ij= pow(_i+1, _j-1);
      matrix_setAt(A, i, j, a_ij);
      j++;
    }
    i++;
  }
}

int solveForN(int N) {
  Matrix* A = matrix_new(N, N);
  Matrix* X = matrix_new(N, 1);
  Matrix* B = matrix_new(N, 1);

  setMatrixNGE(A, B);

  Matrix* A_copy = matrix_newCopy(A);
  Matrix* B_copy = matrix_newCopy(B);

  printf("matriz A:\n");
  matrix_print(A, 2);
  printf("vetor B:\n");
  matrix_print(B, 2);

  bool ok = matrix_gaussJordan(A, B, ERROR);
  if (!ok) {
    printf("Gauss-Jordan falhou! Certeza que a matriz é inversível?\n");
    abort();
  }
  printf("matriz A (gauss-jordan):\n");
  matrix_print(A, 2);
  printf("vetor B (gauss-jordan):\n");
  matrix_print(B, 2);

  // a solução é o B!
  matrix_copy(B, X);

  /* Verifica se a solução resolve o problema original, antes da eliminação gaussiana. */
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

int main(void) {
  int n = 4;
  while (n < 9) {
    solveForN(n);
    printf("\n\n\n");
    n++;
  }
}
