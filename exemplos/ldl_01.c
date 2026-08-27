#include "../lib/matrix.h"
#include "../lib/06_lu.h"

#include <stdio.h>
#include <stdlib.h>

#define PRECISION 5
#define ERROR 1e-3

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

void setMatrixSym(Matrix* A) {
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

  // cria uma matriz simétrica em A (redefine A = A^TA
  Matrix* AT = matrix_new(A->rows, A->columns);
  Matrix* Sym = matrix_new(A->rows, A->columns);

  matrix_transpose(A, AT);
  matrix_mult(A, AT, Sym);
  matrix_copy(Sym, A);

  matrix_free(&AT);
  matrix_free(&Sym);
}

void solve(int ORDER) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* L = matrix_new(ORDER, ORDER);
  Matrix* D = matrix_new(ORDER, ORDER);
  bool ok;

  setMatrixSym(A);
  Matrix* A_copy = matrix_newCopy(A);

  ok = matrix_NaiveLDLDecomposition(A, L, D, 1e-10);
  if (ok == false) {
    printf("Decomposição LDL falhou!\n");
    matrix_free(&A);
    matrix_free(&L);
    matrix_free(&D);
    matrix_free(&A_copy);
    return;
  }

  {
    Matrix* L_T = matrix_new(ORDER, ORDER);
    Matrix* LD = matrix_new(ORDER, ORDER);
    Matrix* LDL = matrix_new(ORDER, ORDER);

    matrix_transpose(L, L_T);
    matrix_mult(L, D, LD);
    matrix_mult(LD, L_T, LDL);

    if(matrix_equals(A_copy, LDL, ERROR)) {
      printf("Decomposição LDL foi um sucesso!\n");
    } else {
      printf("Decomposição LDL falhou!\n");
      matrix_print(A_copy, PRECISION);
      printf("\n");
      matrix_print(LDL, PRECISION);
      printf("---------------\n");
    }

    matrix_free(&L_T);
    matrix_free(&LD);
    matrix_free(&LDL);
  }

  matrix_free(&A);
  matrix_free(&A_copy);
  matrix_free(&L);
  matrix_free(&D);
}

int main(void) {
  int i = 4;
  while (i < 8) {
    solve(i);
    i++;
  }
}
