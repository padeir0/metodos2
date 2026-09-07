#include "matrix.h"

#include <float.h>

#ifndef M2_SOR_H
#define M2_SOR_H

/* Acha a solução aproximada do sistema `Ax = B`,
   usa `|X_1 - X_2|^2 < tolerance^2` como condição
   de parada, onde a norma é a norma de Frobenius.
   A função retorna o número total de iterações.
   Se `maxIter` for atingida, é provável que o resultado
   obtido esteja fora da tolerância.

   É apenas uma variação do algoritmo de Jacobi.
*/
int matrix_SOR(const Matrix* A, const Matrix* B, Matrix* X, int maxIter, double tolerance, double WRelax) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(X != NULL);
    assert(matrix_isValidLinearSystem(A, X, B));
    assert(0 <= WRelax && WRelax < 2);
  #endif
  Matrix* NewX = matrix_new(X->rows, X->columns);
  Matrix* X_1 = X;
  Matrix* X_2 = NewX;

  int iter = 0;
  while (iter < maxIter) {
    int i = 0;
    while (i < A->rows) {
      double a_ii = matrix_at(A, i, i);
      double value = matrix_at(B, i, 0);
      int j = 0;
      while (j < A->columns) {
        double a_ij = matrix_at(A, i, j);

        if (j < i) { // nesse caso, o valor x_j já foi computado
          double x_j = matrix_at(X_2, j, 0);
          value -= a_ij * x_j;
        } else if (i < j) { // aqui o valor x_j ainda não foi computado (pegamos da iteração anterior)
          double x_j = matrix_at(X_1, j, 0);
          value -= a_ij * x_j;
        } // não fazemos nada caso i == j
        j++;
      }
      value *= WRelax / a_ii;
      value += matrix_at(X_1, i, 0) * (1 - WRelax);
      matrix_setAt(X_2, i, 0, value);
      i++;
    }

    if (matrix_distanceSquared(X_1, X_2) < tolerance*tolerance) {
      break;
    }

    { // swap
      Matrix* Z = X_1;
      X_1 = X_2;
      X_2 = Z;
    }
    iter++;
  }

  if (X != X_2) {
    matrix_copy(X_2, X);
  }
  matrix_free(&NewX);
  return iter;
}

#endif
