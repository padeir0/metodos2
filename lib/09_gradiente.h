#include "matrix.h"

#include <float.h>

#ifndef M2_GRADIENTE_H
#define M2_GRADIENTE_H

/* Acha a solução do sistema Ax = B usando o método do gradiente.
   Aloca duas matrizes como scratch space.
*/
int matrix_gradient(const Matrix* A, Matrix* X, const Matrix* B, int itermax, double tol) {
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

/* Acha a solução do sistema Ax = B usando o método do gradiente conjugado.
   Aloca três (!) matrizes como scratch space.
   A implementação ta meio bagunçada pq eu fiz "alocação de registradores" mentalmente
   pra usar o mínimo de scratch space que consegui.
*/
int matrix_conjugateGradient(const Matrix* A, Matrix* X, const Matrix* B, int itermax, double tol) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(X != NULL);
    assert(tol >= 0);
    assert(matrix_isValidLinearSystem(A, X, B));
  #endif

  Matrix* r = matrix_new(X->rows, X->columns);

  // scratch space
  Matrix* C = matrix_new(X->rows, X->columns);
  Matrix* D = matrix_new(X->rows, X->columns);

  // r := B - AX
  matrix_mult(A, X, C); // C = AX
  matrix_sub(B, C, r);
  if (matrix_normSquared(r) < tol*tol) {
    matrix_free(&r);
    matrix_free(&C);
    matrix_free(&D);
    return 0;
  }

  Matrix* p = matrix_newCopy(r);

  int i = 0;
  do {
    matrix_mult(A, p, C); // C = Ap
    // alpha = <r, r> / <Ap, p>
    double alpha = matrix_innerProduct(r, r) / matrix_innerProduct(C, p);

    matrix_scalarMult(p, alpha, C); // C = alpha*p
    matrix_add(X, C, X);            // X += alpha*p

    matrix_mult(A, C, D); // D = A*(alpha*p)
    matrix_sub(r, D, C); // C = r_{k+1} = r_k - A*(alpha*p)
    if (matrix_normSquared(C) < tol*tol) {
      break;
    }

    // beta = <r_{k+1}, r_{k+1}> / <r, r>
    double beta = matrix_innerProduct(C, C) / matrix_innerProduct(r, r);
    matrix_scalarMult(p, beta, p); // p *= beta
    matrix_add(p, C, p);           // p += r_{k+1}
    matrix_copy(C, r); // r_k = r_{k+1}
    i++;
  } while (i < itermax);

  matrix_free(&r);
  matrix_free(&C);
  matrix_free(&D);
  matrix_free(&p);
  return i;
}

#endif
