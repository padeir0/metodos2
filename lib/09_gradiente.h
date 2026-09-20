#include "matrix.h"

#include <float.h>

#ifndef M2_GRADIENTE_H
#define M2_GRADIENTE_H

/* Acha a solução do sistema Ax = B usando o método do gradiente.
   Aloca duas matrizes como scratch space.
*/
int matrix_gradientSteepestDescent(const Matrix* A, Matrix* X, const Matrix* B, int itermax, double tol) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(X != NULL);
    assert(tol >= 0);
    assert(matrix_isValidLinearSystem(A, X, B));
  #endif

  Matrix* r = matrix_new(X->rows, X->columns);
  Matrix* scratch = matrix_new(X->rows, X->columns);
  int i = 0;
  do {
    // r := B - AX
    matrix_mult(A, X, scratch);
    matrix_sub(B, scratch, r);

    if (matrix_normSquared(r) < tol*tol) {
      break;
    }
    matrix_mult(A, r, scratch);
    double s = matrix_normSquared(r) / matrix_innerProduct(scratch, r);
    matrix_scalarMult(r, s, r);
    matrix_add(X, r, X);
    i++;
  } while (i < itermax);

  matrix_free(&r);
  matrix_free(&scratch);
  return i;
}

#endif
