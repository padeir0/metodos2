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
  Matrix* Ap = matrix_new(X->rows, X->columns);
  Matrix* D = matrix_new(X->rows, X->columns);

  // r := B - AX
  matrix_mult(A, X, D); // D := AX
  matrix_sub(B, D, r);

  if (matrix_normSquared(r) < tol*tol) {
    matrix_free(&r);
    matrix_free(&Ap);
    matrix_free(&D);
    return 0;
  }

  Matrix* p = matrix_newCopy(r);

  int i = 0;
  do {
    double r_dot_r = matrix_innerProduct(r, r);

    matrix_mult(A, p, Ap);
    // alpha = <r, r> / <Ap, p>
    double alpha =  r_dot_r / matrix_innerProduct(Ap, p);

    matrix_scalarMult(p, alpha, D); // D = alpha*p
    matrix_add(X, D, X);            // X += alpha*p

    matrix_scalarMult(Ap, alpha, D); // D = alpha*Ap
    matrix_sub(r, D, r); // r_{k+1} = r_k - alpha*Ap
    if (matrix_normSquared(r) < tol*tol) {
      break;
    }

    // beta = <r_{k+1}, r_{k+1}> / <r, r>
    double beta = matrix_innerProduct(r, r) / r_dot_r;
    matrix_scalarMult(p, beta, p); // p *= beta
    matrix_add(p, r, p);          // p += r_{k+1}
    i++;
  } while (i < itermax);

  matrix_free(&r);
  matrix_free(&Ap);
  matrix_free(&D);
  matrix_free(&p);
  return i;
}

#endif
