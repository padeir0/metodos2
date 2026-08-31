/* Testes unitários pra `04_tridiagonal.h`: matrix_solveTridiagonal.

   Convenção das 3 diagonais (igual a exemplos/tridiagonal_01.c): A[i] é a
   subdiagonal (usada só pra i>=1), B[i] é a diagonal principal, C[i] é a
   sobrediagonal (usada só pra i<n-1). A matriz completa equivalente tem
   A_full[i][i]=B[i], A_full[i][i-1]=A[i] e A_full[i-1][i]=C[i-1].

   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 04_tridiagonal_test.c -o out_tridiagonaltest
*/

#include "common.h"
#include "../lib/01_subst.h"
#include "../lib/04_tridiagonal.h"

/* monta a matriz completa NxN a partir das 3 diagonais, pra poder checar a
   solução com matrix_verifySolution (mesma ideia de recreateFullSystem em
   exemplos/tridiagonal_01.c). */
void buildFullSystem(const Matrix* A, const Matrix* B, const Matrix* C, Matrix* full) {
  int n = B->rows;
  int i = 0;
  while (i < n) {
    matrix_setAt(full, i, i, matrix_at(B, i, 0));
    i++;
  }
  i = 1;
  while (i < n) {
    matrix_setAt(full, i, i-1, matrix_at(A, i, 0));
    matrix_setAt(full, i-1, i, matrix_at(C, i-1, 0));
    i++;
  }
}

/* BEGIN: testing matrix_solveTridiagonal */

// caso 1x1: não entra no loop de eliminação (i=1, 1<1 falso) nem no loop
// de volta (i=n-1=-1, -1>=0 falso) -- só divide D[0] por B[0].
bool test_solveTridiagonal_oneByOne(void) {
  Matrix* a = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, 0.0); // não usado
  Matrix* b = matrix_new(1, 1);
  matrix_setAt(b, 0, 0, 5.0);
  Matrix* c = matrix_new(1, 1);
  matrix_setAt(c, 0, 0, 0.0); // não usado
  Matrix* d = matrix_new(1, 1);
  matrix_setAt(d, 0, 0, 15.0);
  Matrix* x = matrix_new(1, 1);

  matrix_solveTridiagonal(a, b, c, x, d);
  double expected[] = {3.0}; // 15/5

  bool ok = expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  matrix_free(&d);
  matrix_free(&x);
  return ok;
}

// sistema 3x3 com solução exata conhecida X*=[1,2,3] (calculado à mão,
// ver comentário no styleguide de testes do repo).
bool test_solveTridiagonal_threeByThree(void) {
  Matrix* a = matrix_new(3, 1);
  double avals[] = {0, 1, 1}; // A[0] não é usado
  matrix_set(avals, 3, 1, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {4, 4, 4};
  matrix_set(bvals, 3, 1, b);
  Matrix* c = matrix_new(3, 1);
  double cvals[] = {1, 1, 0}; // C[2] não é usado
  matrix_set(cvals, 3, 1, c);
  Matrix* d = matrix_new(3, 1);
  double dvals[] = {6, 12, 14};
  matrix_set(dvals, 3, 1, d);
  Matrix* x = matrix_new(3, 1);

  Matrix* full = matrix_new(3, 3);
  buildFullSystem(a, b, c, full);
  Matrix* dOrig = matrix_newCopy(d);

  matrix_solveTridiagonal(a, b, c, x, d);
  double expected[] = {1, 2, 3};

  bool ok = expectMatrix(x, expected, 1e-9) &&
            matrix_verifySolution(full, x, dOrig, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  matrix_free(&d);
  matrix_free(&x);
  matrix_free(&full);
  matrix_free(&dOrig);
  return ok;
}

// documenta o "Modifica os inputs!" do docstring: depois da chamada, B e D
// não são mais os valores originais (foram sobrescritos pela eliminação
// direta), enquanto A e C permanecem intocados. Mesmos valores de
// test_solveTridiagonal_threeByThree, conferidos à mão.
bool test_solveTridiagonal_overwritesBAndDInPlace(void) {
  Matrix* a = matrix_new(3, 1);
  double avals[] = {0, 1, 1};
  matrix_set(avals, 3, 1, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {4, 4, 4};
  matrix_set(bvals, 3, 1, b);
  Matrix* c = matrix_new(3, 1);
  double cvals[] = {1, 1, 0};
  matrix_set(cvals, 3, 1, c);
  Matrix* d = matrix_new(3, 1);
  double dvals[] = {6, 12, 14};
  matrix_set(dvals, 3, 1, d);
  Matrix* x = matrix_new(3, 1);

  Matrix* aOrig = matrix_newCopy(a);
  Matrix* cOrig = matrix_newCopy(c);

  matrix_solveTridiagonal(a, b, c, x, d);

  double expectedB[] = {4, 3.75, 56.0/15.0}; // 4 - (1/3.75)*1 = 3.7333... = 56/15
  double expectedD[] = {6, 10.5, 11.2};

  bool ok = expectMatrix(b, expectedB, 1e-9) &&
            expectMatrix(d, expectedD, 1e-9) &&
            matrix_equals(a, aOrig, 1e-9) && // A não muda
            matrix_equals(c, cOrig, 1e-9);   // C não muda

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  matrix_free(&d);
  matrix_free(&x);
  matrix_free(&aOrig);
  matrix_free(&cOrig);
  return ok;
}

// outro tamanho (2x2), solução exata X*=[2,4], pra cobrir o caso em que o
// loop de eliminação/retrocesso roda uma única vez em vez de duas.
bool test_solveTridiagonal_twoByTwo(void) {
  Matrix* a = matrix_new(2, 1);
  double avals[] = {0, 1};
  matrix_set(avals, 2, 1, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {3, 3};
  matrix_set(bvals, 2, 1, b);
  Matrix* c = matrix_new(2, 1);
  double cvals[] = {1, 0};
  matrix_set(cvals, 2, 1, c);
  Matrix* d = matrix_new(2, 1);
  double dvals[] = {10, 14};
  matrix_set(dvals, 2, 1, d);
  Matrix* x = matrix_new(2, 1);

  Matrix* full = matrix_new(2, 2);
  buildFullSystem(a, b, c, full);
  Matrix* dOrig = matrix_newCopy(d);

  matrix_solveTridiagonal(a, b, c, x, d);
  double expected[] = {2, 4};

  bool ok = expectMatrix(x, expected, 1e-9) &&
            matrix_verifySolution(full, x, dOrig, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  matrix_free(&d);
  matrix_free(&x);
  matrix_free(&full);
  matrix_free(&dOrig);
  return ok;
}
/* END: testing matrix_solveTridiagonal */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_solveTridiagonal_oneByOne", test_solveTridiagonal_oneByOne},
  {"test_solveTridiagonal_threeByThree", test_solveTridiagonal_threeByThree},
  {"test_solveTridiagonal_overwritesBAndDInPlace", test_solveTridiagonal_overwritesBAndDInPlace},
  {"test_solveTridiagonal_twoByTwo", test_solveTridiagonal_twoByTwo},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("04_tridiagonal", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
