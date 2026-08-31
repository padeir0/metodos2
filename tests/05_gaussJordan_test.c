/* Testes unitários pra `05_gaussJordan.h`: matrix_gaussJordan.

   A função interna i_matrix_findLargestRow (idêntica em código à de
   03_gaussPivot.h, mas uma cópia `static` própria deste arquivo) não é
   testada diretamente aqui -- só indiretamente, através dos testes de
   matrix_gaussJordan abaixo (caso de pivô nulo força a busca a escolher
   outra linha).

   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 05_gaussJordan_test.c -o out_gaussjordantest
*/

#include "common.h"
#include "../lib/01_subst.h"
#include "../lib/05_gaussJordan.h"

/* BEGIN: testing matrix_gaussJordan */

// caso básico, sem pivô nulo na diagonal. Solução exata X*=[1,2,1]
// (conferida com numpy: A@[1,2,1] = [5,8,7]).
bool test_gaussJordan_basicReducesToIdentityAndSolves(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    2, 1, 1,
    1, 3, 1,
    1, 1, 4,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {5, 8, 7};
  matrix_set(bvals, 3, 1, b);

  Matrix* aOrig = matrix_newCopy(a);
  Matrix* bOrig = matrix_newCopy(b);

  bool result = matrix_gaussJordan(a, b, 1e-9);

  double expectedA[] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
  };
  double expectedX[] = {1, 2, 1};

  bool ok = result &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedX, 1e-9) &&
            matrix_verifySolution(aOrig, b, bOrig, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&aOrig);
  matrix_free(&bOrig);
  return ok;
}

// pivô nulo na diagonal força i_matrix_findLargestRow a escolher outra
// linha (a única com entrada não-nula na coluna 0). Solução conferida
// com numpy.linalg.solve: X = [5/3, 2].
bool test_gaussJordan_pivotsOnZeroDiagonal(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    0, 2,
    3, 1,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {4, 7};
  matrix_set(bvals, 2, 1, b);

  Matrix* aOrig = matrix_newCopy(a);
  Matrix* bOrig = matrix_newCopy(b);

  bool result = matrix_gaussJordan(a, b, 1e-9);

  double expectedA[] = {
    1, 0,
    0, 1,
  };
  double expectedX[] = {5.0/3.0, 2.0};

  bool ok = result &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedX, 1e-9) &&
            matrix_verifySolution(aOrig, b, bOrig, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&aOrig);
  matrix_free(&bOrig);
  return ok;
}

// entrada já identidade: nenhuma eliminação de fato muda nada, B
// permanece igual (X = B, já que A = I).
bool test_gaussJordan_identityInputIsNoOp(void) {
  Matrix* a = matrix_new(2, 2);
  matrix_setIdentity(a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {5, 7};
  matrix_set(bvals, 2, 1, b);

  bool result = matrix_gaussJordan(a, b, 1e-9);

  double expectedA[] = {1, 0, 0, 1};
  double expectedX[] = {5, 7};

  bool ok = result &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedX, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// matriz singular (linha 1 é múltipla da linha 0): depois de zerar a
// coluna 0 na linha 1, a busca por pivô na coluna 1 (a partir da linha 1)
// falha -> retorna false. Também serve de regressão para o vazamento de
// `AB` que existia nesse ramo (corrigido em lib/05_gaussJordan.h): com
// -fsanitize=address,undefined, um leak faria esse teste "vazar" no
// LeakSanitizer mesmo que o valor de retorno estivesse certo.
bool test_gaussJordan_falseForSingularMatrix(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    1, 2,
    2, 4,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {1, 1};
  matrix_set(bvals, 2, 1, b);

  bool ok = matrix_gaussJordan(a, b, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}
/* END: testing matrix_gaussJordan */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_gaussJordan_basicReducesToIdentityAndSolves", test_gaussJordan_basicReducesToIdentityAndSolves},
  {"test_gaussJordan_pivotsOnZeroDiagonal", test_gaussJordan_pivotsOnZeroDiagonal},
  {"test_gaussJordan_identityInputIsNoOp", test_gaussJordan_identityInputIsNoOp},
  {"test_gaussJordan_falseForSingularMatrix", test_gaussJordan_falseForSingularMatrix},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("05_gaussJordan", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
