#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/07_jacobiGaussSeidel.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR 1e-9
#define MAXITER 1000
#define ORDER 4

void check(bool ok) {
  if (ok == false) {
    printf("algo de errado não está certo\n");
    abort();
  }
}

double A_data[ORDER*ORDER] = {
  8.0, 3.0, 2.0, 1,
    0, 3.0,   1, 1,
    0,   0,   1, 0,
   -1,   0,   0, 4,
};

int main(void) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* X = matrix_new(ORDER, 1);
  Matrix* B = matrix_new(ORDER, 1);
  Matrix* solution = matrix_new(ORDER, 1);

  { // seta o sistema pra solução ser X = [1, 1, 1, ..., 1]
    matrix_set(A_data, ORDER, ORDER, A);
    matrix_setAll(solution, 1);
    matrix_mult(A, solution, B);
    matrix_setAll(X, 0);
  }

  printf("matriz A:\n");
  matrix_print(A, 2);
  printf("vetor B:\n");
  matrix_print(B, 2);

  int iter = matrix_jacobi(A, B, X, MAXITER, ERROR);
  if (iter == MAXITER) {
    printf("Método de Jacobi não convergiu!\n");
    printf("matriz X:\n");
    matrix_print(X, 2);
    abort();
  }

  printf("matriz X:\n");
  matrix_print(X, 9);
  printf("número de iterações: %d\n", iter);

  bool solved = matrix_verifySolution(A, X, B, ERROR);
  if (solved) {
    printf("Solução correta!\n");
  } else {
    printf("Solução incorreta!\n");
  }

  matrix_free(&A);
  matrix_free(&X);
  matrix_free(&B);
  matrix_free(&solution);
  return 0;
}
