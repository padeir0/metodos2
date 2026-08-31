#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/07_jacobiGaussSeidel.h"

#include <stdio.h>
#include <stdlib.h>

// tolerancia de convergência pros algoritmos
#define TOLERANCE 1e-12
// erro máximo aceito na solução do sistema
#define ERROR 1e-6
// número de casas decimais para imprimir as matrizes
#define PRECISION 6
// número máximo de iterações dos algoritmos
#define MAXITER 1000

// pra gerar a matriz aleatória
#define COEFRANGE 1000
#define COEFMIN -500

// cria um sistema pseudo-aleatório com diagonal dominante
void createLinearSystem(Matrix* A, Matrix* X, Matrix* B) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(A->rows == A->columns);
    assert(B->rows == A->columns);
    assert(X->rows = A->columns);
    assert(X->columns == 1);
    assert(B->columns == 1);
  #endif
  srand(42);

  matrix_setAll(X, 1);
  
  int N = A->rows;
  int i = 0;
  while (i < N) {
    int j = 0;
    while (j < N) {
      if (i != j) {
        double a_ij = rand() % COEFRANGE + COEFMIN;
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
    matrix_setAt(A, i, i, sum);

    i++;
  }

  matrix_mult(A, X, B);
}

void solveForN(int ORDER) {
  Matrix* A = matrix_new(ORDER, ORDER);
  Matrix* X_jacobi = matrix_new(ORDER, 1);
  Matrix* X_gaussSeidel = matrix_new(ORDER, 1);
  Matrix* B = matrix_new(ORDER, 1);
  Matrix* solution = matrix_new(ORDER, 1);

  createLinearSystem(A, solution, B);
  matrix_setAll(X_jacobi, 2);
  matrix_setAll(X_gaussSeidel, 2);

  int iter_jacobi = matrix_jacobi(A, B, X_jacobi, MAXITER, TOLERANCE);
  if (iter_jacobi == MAXITER) {
    printf("Método de Jacobi não convergiu!\n");
    printf("matriz X:\n");
    matrix_print(X_jacobi, PRECISION);
    abort();
  }

  int iter_gaussSeidel = matrix_gaussSeidel(A, B, X_gaussSeidel, MAXITER, TOLERANCE);
  if (iter_gaussSeidel == MAXITER) {
    printf("Método de Gauss-Seidel não convergiu!\n");
    printf("matriz X:\n");
    matrix_print(X_gaussSeidel, PRECISION);
    abort();
  }

  bool solved_jacobi = matrix_verifySolution(A, X_jacobi, B, ERROR);
  bool solved_gaussSeidel = matrix_verifySolution(A, X_gaussSeidel, B, ERROR);

  if (solved_jacobi && solved_gaussSeidel) {
    printf("número de iterações (Jacobi): %d\n", iter_jacobi);
    printf("número de iterações (Gauss-Seidel): %d\n", iter_gaussSeidel);
  } else {
    printf("um dos dois métodos não solucionou o sistema!\n");
    printf("Solução (Jacobi):\n");
    matrix_print(X_jacobi, PRECISION);
    printf("Solução (Gauss-Seidel):\n");
    matrix_print(X_gaussSeidel, PRECISION);
  }

  matrix_free(&A);
  matrix_free(&X_jacobi);
  matrix_free(&X_gaussSeidel);
  matrix_free(&B);
  matrix_free(&solution);
}

int main(void) {
  int i = 4;
  while (i < 10) {
    solveForN(i);
    i++;
  }
}
