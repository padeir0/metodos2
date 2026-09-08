#include "../lib/matrix.h"
#include "../lib/01_subst.h"
#include "../lib/03_gaussPivot.h"
#include "../lib/07_jacobiGaussSeidel.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// número máximo de iterações do jacobi
#define MAXITER 1000

// tolerância de convergência do jacobi (critério |X_1 - X_2| <= JACOBI_TOL)
#define JACOBI_TOL 1e-8

// tolerância usada pro pivotamento/substituição do gauss (detecção de
// pivô/diagonal nula), não é uma medida de precisão da solução
#define PIVOT_ERROR 1e-8

// pra gerar a matriz aleatória
#define COEFRANGE 1000
#define COEFMIN -500

/* cria um sistema pseudo-aleatório com diagonal dominante */
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
    matrix_setAt(A, i, i, 2*sum+1);

    i++;
  }

  matrix_mult(A, X, B);
}

void performComparison(int order) {
  Matrix* A = matrix_new(order, order);
  Matrix* X = matrix_new(order, 1);
  Matrix* B = matrix_new(order, 1);

  createLinearSystem(A, X, B);

  Matrix* X_jacobi = matrix_new(order, 1);
  matrix_setAll(X_jacobi, 2);

  clock_t start_jacobi = clock();
  int iter_jacobi = matrix_jacobi(A, B, X_jacobi, MAXITER, JACOBI_TOL);
  clock_t end_jacobi = clock();

  if (iter_jacobi == MAXITER) {
    fprintf(stderr,
            "aviso: jacobi não convergiu (ordem %d, tol=%.0e)\n",
            order, JACOBI_TOL);
  }

  Matrix* A_gaussPivot = matrix_newCopy(A);
  Matrix* B_gaussPivot = matrix_newCopy(B);
  Matrix* X_gaussPivot = matrix_new(order, 1);

  clock_t start_gaussPivot = clock();
  bool ok = matrix_sqGaussianEliminationPivot_2(A_gaussPivot, B_gaussPivot, PIVOT_ERROR);
  if (!ok) {
    printf("Eliminação c/ pivotamento falhou pra ordem %d! Certeza que a matriz é inversível?\n", order);
    abort();
  }
  ok = matrix_solveBySubstitution(A_gaussPivot, X_gaussPivot, B_gaussPivot, PIVOT_ERROR);
  if (!ok) {
    printf("Não deu pra solucionar a matriz (ordem %d)! Certeza que ela é triangular?\n", order);
    matrix_print(A_gaussPivot, 8);
    abort();
  }
  clock_t end_gaussPivot = clock();

  double time_jacobi = ((double)(end_jacobi - start_jacobi)) / CLOCKS_PER_SEC;
  double time_gaussPivot = ((double)(end_gaussPivot - start_gaussPivot)) / CLOCKS_PER_SEC;

  printf("%d, %f, %f\n", order, time_jacobi, time_gaussPivot);

  matrix_free(&A);
  matrix_free(&X);
  matrix_free(&B);
  matrix_free(&X_jacobi);
  matrix_free(&A_gaussPivot);
  matrix_free(&B_gaussPivot);
  matrix_free(&X_gaussPivot);
}

int main(void) {
  printf("# ORDER, JACOBI TIME (seconds), GAUSSPIVOT TIME (seconds)\n");
  int order = 2;
  while (order < 2100) {
    performComparison(order);
    order *= 2;
  }
  return 0;
}
