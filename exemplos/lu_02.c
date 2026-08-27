#include "../lib/matrix.h"
#include "../lib/06_lu.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR 1e-9

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

void setMatrixNGE(Matrix* A) {
  #if DEBUG
    assert(A != NULL);
    assert(A->rows == A->columns);
  #endif
  int N = A->rows;
  int i = 0;
  while (i < N) {
    // o algoritmo original ta escrito em fortran (1 <= i <= n)
    double _i = (double)i+1;
    
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

void solve(int ORDER) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* P = matrix_new(ORDER, ORDER);
  Matrix* L = matrix_new(ORDER, ORDER);
  Matrix* U = matrix_new(ORDER, ORDER);
  bool ok;

  setMatrixNGE(A);

  ok = matrix_NaiveLUDecomposition(A, L, U, ERROR);
  if (!ok) {
    printf("Ordem: %d, Decomposição LU ingênua falhou! Certeza que a matriz é inversível?\n", ORDER);
    abort();
  }
  // verifica se A = LU
  {
    Matrix* LU = matrix_new(ORDER, ORDER);
    matrix_mult(L, U, LU);

    if (matrix_equals(A, LU, ERROR)) {
      printf("Ordem: %d, Decomposição LU ingênua: OK!\n", ORDER);
    } else {
      printf("Ordem: %d, Decomposição LU ingênua: FALHOU!\n", ORDER);
    }
    matrix_free(&LU);
  }

  ok = matrix_PivotingLUDecomposition(A, P, L, U, ERROR);
  if (!ok) {
    printf("Ordem: %d, Decomposição LU c/ pivotamento falhou! Certeza que a matriz é inversível?\n", ORDER);
    abort();
  }
  // verifica se PA = LU
  {
    Matrix* PA = matrix_new(ORDER, ORDER);
    Matrix* LU = matrix_new(ORDER, ORDER);
    matrix_mult(P, A, PA);
    matrix_mult(L, U, LU);

    if (matrix_equals(PA, LU, ERROR)) {
      printf("Ordem: %d, Decomposição LU c/ pivotamento: OK!\n", ORDER);
    } else {
      printf("Ordem: %d, Decomposição LU c/ pivotamento: FALHOU!\n", ORDER);
    }
    matrix_free(&PA);
    matrix_free(&LU);
  }

  matrix_free(&P);
  matrix_free(&A);
  matrix_free(&L);
  matrix_free(&U);
}

int main(void) {
  int i = 4;
  while (i < 10) {
    solve(i);
    i++;
  }
}
