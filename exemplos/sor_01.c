#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/07_jacobiGaussSeidel.h"
#include "../lib/08_SOR.h"

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

#define THE_PI 3.14159265358979323846264338327950288419716

// passo de variação do coeficiente de relaxamento
#define WSTEP 0.01

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
    matrix_setAt(A, i, i, 2*sum+1);

    i++;
  }

  matrix_mult(A, X, B);
}

void solve(const Matrix* A, const Matrix* B, double WRELAX) {
  Matrix* X_SOR = matrix_new(A->columns, 1);
  matrix_setAll(X_SOR, THE_PI);

  int iter_SOR = matrix_SOR(A, B, X_SOR, MAXITER, TOLERANCE, WRELAX);
  if (iter_SOR == MAXITER) {
    printf("W = %f, Método de SOR não convergiu!\n", WRELAX);
    matrix_free(&X_SOR);
    return;
  }

  bool solved_SOR = matrix_verifySolution(A, X_SOR, B, ERROR);

  if (solved_SOR) {
    printf("W = %f, número de iterações (SOR): %d\n", WRELAX, iter_SOR);
  } else {
    printf("W = %f, O método não solucionou o sistema.\n", WRELAX);
  }
  matrix_free(&X_SOR);
  return;
}

int main(void) {
  int ORDER = 4;
  while (ORDER < 10) {
    Matrix* A = matrix_new(ORDER, ORDER);
    Matrix* solution = matrix_new(ORDER, 1);
    Matrix* B = matrix_new(ORDER, 1);
    matrix_setAll(solution, 1);
    createLinearSystem(A, solution, B);

    printf("\nMatrizes de ordem %d:\n", ORDER);
    double wrelax = 0.9;
    while (wrelax < 1.1) {
      solve(A, B, wrelax);
      wrelax += WSTEP;
    }

    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&solution);
    ORDER++;
  }
}
