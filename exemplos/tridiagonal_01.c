#include "../lib/matrix.h"
#include "../lib/03_gaussPivot.h"
#include "../lib/04_tridiagonal.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR 1e-9
#define ORDER 5
#define NUMMOLAS 6

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

double K_data[NUMMOLAS] = {8, 9, 15, 12, 10, 18};
double L_data[NUMMOLAS] = {0.18, 0.22, 0.26, 0.19, 0.15, 0.30};

void setSystem(Matrix* A, Matrix* B, Matrix* C, Matrix* D) {
  /* preenche B (diagonal) */
  int i = 0;
  while (i < A->rows) {
    double sum = K_data[i] + K_data[i+1];
    matrix_setAt(B, i, 0, sum);
    i++;
  }

  /* preenche A (abaixo da diagonal) e C (acima da diagonal) */
  i = 1;
  while (i < A->rows) {
    double k_i = K_data[i];
    matrix_setAt(A, i, 0, -k_i);
    matrix_setAt(C, i-1, 0, -k_i);
    i++;
  }

  i = 0;
  while (i < D->rows-1) {
    double value = K_data[i] * L_data[i] + K_data[i+1] * L_data[i+1];
    matrix_setAt(D, i, 0, value);
    i++;
  }

  double L = 0;
  i = 0;
  while (i < NUMMOLAS) {
    L += L_data[i];
    i++;
  }
  
  int n = NUMMOLAS-1;
  double value = K_data[n-1]*L_data[n-1] + K_data[n]*L + K_data[n]*L_data[n];
  matrix_setAt(D, D->rows-1, 0, value);
}

/* pega as diagonais e transforma em um sistema matricial completo Ax = B*/
void recreateFullSystem(const Matrix* A,
                        const Matrix* B,
                        const Matrix* C,
                        const Matrix* D, Matrix* A_full, Matrix* B_full) {
  int order = D->rows;
  matrix_setIdentity(A_full);

  int i = 0;
  while (i < order) {
    double b_i = matrix_at(B, i, 0);
    matrix_setAt(A_full, i, i, b_i);
    i++;
  }
  i = 1;
  while (i < order) {
    double a_i = matrix_at(A, i, 0);
    double c_i = matrix_at(C, i-1, 0);
    matrix_setAt(A_full, i, i-1, a_i);
    matrix_setAt(A_full, i-1, i, c_i);
    i++;
  }

  matrix_copy(D, B_full);
}

int main(void) {
  /* as 3 diagonais A, B, C*/
  Matrix* A = matrix_new(ORDER, 1);
  Matrix* B = matrix_new(ORDER, 1);
  Matrix* C = matrix_new(ORDER, 1);
  /* os fatores depois da igualdade Ax = D*/
  Matrix* D = matrix_new(ORDER, 1);
  /* a matrix de variáveis */
  Matrix* X = matrix_new(ORDER, 1);

  /* o sistema completo em forma de matriz*/
  Matrix* A_full = matrix_new(ORDER, ORDER);
  Matrix* B_full = matrix_new(ORDER, 1);

  setSystem(A, B, C, D);
  recreateFullSystem(A, B, C, D, A_full, B_full);

  matrix_solveTridiagonal(A, B, C, X, D);
  printf("solução tridiagonal:\n");
  matrix_print(X, 2);

  bool ok = matrix_verifySolution(A_full, X, B_full, ERROR);
  if (ok) {
    printf("solução correta!\n");
  } else {
    printf("solução incorreta!\n");
  }

  matrix_free(&A);
  matrix_free(&B);
  matrix_free(&C);
  matrix_free(&D);
  matrix_free(&X);
  matrix_free(&A_full);
  matrix_free(&B_full);
  return 0;
}
