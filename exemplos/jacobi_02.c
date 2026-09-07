#include "../lib/matrix.h"
#include "../lib/07_jacobiGaussSeidel.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// número máximo de iterações do algoritmo
#define MAXITER 1000

// pra gerar a matriz aleatória
#define MANTISSA_RANGE 256
#define MANTISSA_MIN -128
#define EXPO_RANGE 4
#define EXPO_MIN -2

// cria um sistema pseudo-aleatório com diagonal dominante
void createLinearSystem(Matrix* A, Matrix* X, Matrix* B) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(A->rows == A->columns);
    assert(B->rows == A->columns);
    assert(X->rows == A->columns);
    assert(X->columns == 1);
    assert(B->columns == 1);
  #endif
  matrix_setAll(X, 1);
  
  int N = A->rows;
  int i = 0;
  while (i < N) {
    int j = 0;
    while (j < N) {
      if (i != j) {
        double mantissa = rand() % MANTISSA_RANGE + MANTISSA_MIN;
        double expo = rand() % EXPO_RANGE + EXPO_MIN;
        double a_ij = mantissa * pow(10, expo);
        matrix_setAt(A, i, j, a_ij);
      }
      j++;
    }

    j = 0;
    double sum = 0;
    while (j < N) {
      sum += fabs(matrix_at(A, i, j));
      j++;
    }
    /* Tomamos 2*sum+1 pra garantir que o sistema é ESTRITAMENTE
       diagonal dominante. As constantes `2` e `1` são arbitrárias.
    */
    matrix_setAt(A, i, i, 2*sum + 1);

    i++;
  }

  matrix_mult(A, X, B);
}

int solveForN(int order, double tol) {
  int prec = (int)fabs(ceil(log10(tol)));

  Matrix* A = matrix_new(order, order);
  Matrix* X_jacobi = matrix_new(order, 1);
  Matrix* B = matrix_new(order, 1);
  Matrix* solution = matrix_new(order, 1);

  createLinearSystem(A, solution, B);
  matrix_setAll(X_jacobi, 2);

  int iter_jacobi = matrix_jacobi(A, B, X_jacobi, MAXITER, tol);
  if (iter_jacobi == MAXITER) {
    printf("Método de Jacobi não convergiu!\n");
    printf("matriz X:\n");
    matrix_print(X_jacobi, prec);
    abort();
  }

  bool solved_jacobi = matrix_equals(solution, X_jacobi, tol);
  if (solved_jacobi == false) {
    printf("Falhou! Tol: %.*f:\n", prec, tol);
    matrix_print(X_jacobi, prec);
  }

  matrix_free(&A);
  matrix_free(&X_jacobi);
  matrix_free(&B);
  matrix_free(&solution);
  return iter_jacobi;
}

int main(void) {
  srand(42);
  double tol = 1;
  printf("# tol\t\titer\n");
  while (tol > 1e-12) {
    int i = 0;
    double sum = 0;
    while (i < 100) {
      sum += (double)solveForN(8, tol);
      i++;
    }
    int prec = (int)fabs(ceil(log10(tol)));
    printf("%d, %.1f\n", prec, sum/(double)i);
    tol /= 10;
  }
}
