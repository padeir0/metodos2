#include "matrix.h"

#include <float.h>

#ifndef M2_JACOBIGAUSSSEIDEL_H
#define M2_JACOBIGAUSSSEIDEL_H

/* Acha a solução aproximada do sistema `Ax = B`,
   usa `|X_1 - X_2|^2 < tolerance^2` como condição
   de parada, onde a norma é a norma de Frobenius.
   A função retorna o número total de iterações.
   Se `maxIter` for atingida, é provável que o resultado
   obtido esteja fora da tolerância.
*/
int matrix_jacobi(const Matrix* A, const Matrix* B, Matrix* X, int maxIter, double tolerance) {
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
        if (i != j){
          double a_ij = matrix_at(A, i, j);
          double x_j = matrix_at(X_1, j, 0);
          value -= a_ij * x_j;
        }
        j++;
      }
      value = value / a_ii;
      matrix_setAt(X_2, i, 0, value);
      i++;
    }

    // norma de frobenius
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

/* Acha a solução aproximada do sistema `Ax = B`,
   usa `|X_1 - X_2|^2 < tolerance^2` como condição
   de parada, onde a norma é a norma de Frobenius.
   A função retorna o número total de iterações.
   Se `maxIter` for atingida, é provável que o resultado
   obtido esteja fora da tolerância.

   É apenas uma variação do algoritmo de Jacobi.
*/
int matrix_gaussSeidel(const Matrix* A, const Matrix* B, Matrix* X, int maxIter, double tolerance) {
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
      value = value / a_ii;
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
