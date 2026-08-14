/* Testes unitários pra `01_subst.h`: matrix_isLowerTriangular,
   matrix_isUpperTriangular, matrix_solveUpperTriangular,
   matrix_solveLowerTriangular, matrix_solveBySubstitution e
   matrix_verifySolution.
   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 01_subst_test.c -o out_substtest
*/

#include "common.h"
#include "../lib/01_subst.h"

/* BEGIN: testing matrix_isLowerTriangular */
bool test_isLowerTriangular_trueForProperLowerTriangular(void) {
  Matrix* a = matrix_new(3, 3);
  double vals[] = {
    1, 0, 0,
    4, 2, 0,
    7, 8, 3,
  };
  matrix_set(vals, 3, 3, a);

  bool ok = matrix_isLowerTriangular(a, 1e-9);

  matrix_free(&a);
  return ok;
}

// entrada não-nula ACIMA da diagonal reprova.
bool test_isLowerTriangular_falseForNonzeroAboveDiagonal(void) {
  Matrix* a = matrix_new(3, 3);
  double vals[] = {
    1, 0, 0,
    4, 2, 0,
    7, 5, 3, // ok até aqui
  };
  matrix_set(vals, 3, 3, a);
  matrix_setAt(a, 0, 2, 0.5); // corrompe (0,2), acima da diagonal

  bool ok = matrix_isLowerTriangular(a, 1e-9) == false;

  matrix_free(&a);
  return ok;
}

// diagonal "zero demais" (dentro do erro) reprova, mesmo que o
// resto da matriz seja triangular inferior -- a função também
// garante que dá pra usar a matriz em substituição direta.
bool test_isLowerTriangular_falseForNearZeroDiagonalEntry(void) {
  Matrix* a = matrix_new(2, 2);
  double vals[] = {
    1, 0,
    3, 0.0000001,
  };
  matrix_set(vals, 2, 2, a);

  bool ok = matrix_isLowerTriangular(a, 1e-3) == false;

  matrix_free(&a);
  return ok;
}

// diferença exatamente igual a `error` na diagonal já é "zero
// demais" (checagem usa <=): reprova.
bool test_isLowerTriangular_diagonalExactlyAtErrorFails(void) {
  Matrix* a = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, 0.5);

  bool ok = matrix_isLowerTriangular(a, 0.5) == false;

  matrix_free(&a);
  return ok;
}

// diferença exatamente igual a `error` fora da diagonal ainda conta
// como "zero o suficiente" (checagem usa >, não >=): passa.
bool test_isLowerTriangular_offDiagonalExactlyAtErrorPasses(void) {
  Matrix* a = matrix_new(2, 2);
  matrix_setAt(a, 0, 0, 1.0);
  matrix_setAt(a, 0, 1, 0.5); // acima da diagonal, no limite exato do erro
  matrix_setAt(a, 1, 0, 0.0);
  matrix_setAt(a, 1, 1, 1.0);

  bool ok = matrix_isLowerTriangular(a, 0.5);

  matrix_free(&a);
  return ok;
}
/* END: testing matrix_isLowerTriangular */

/* BEGIN: testing matrix_isUpperTriangular */
bool test_isUpperTriangular_trueForProperUpperTriangular(void) {
  Matrix* a = matrix_new(3, 3);
  double vals[] = {
    1, 4, 7,
    0, 2, 8,
    0, 0, 3,
  };
  matrix_set(vals, 3, 3, a);

  bool ok = matrix_isUpperTriangular(a, 1e-9);

  matrix_free(&a);
  return ok;
}

// entrada não-nula ABAIXO da diagonal, em uma coluna != 0, reprova
// (esse caso já era coberto corretamente pela implementação atual).
bool test_isUpperTriangular_falseForNonzeroBelowDiagonal(void) {
  Matrix* a = matrix_new(3, 3);
  double vals[] = {
    1, 4, 7,
    0, 2, 8,
    0, 5, 3, // corrompe (2,1), abaixo da diagonal
  };
  matrix_set(vals, 3, 3, a);

  bool ok = matrix_isUpperTriangular(a, 1e-9) == false;

  matrix_free(&a);
  return ok;
}

bool test_isUpperTriangular_falseForNonzeroInFirstColumn(void) {
  Matrix* a = matrix_new(3, 3);
  matrix_setIdentity(a);
  matrix_setAt(a, 1, 0, 7.0); // abaixo da diagonal, coluna 0

  bool ok = matrix_isUpperTriangular(a, 1e-9) == false;

  matrix_free(&a);
  return ok;
}

bool test_isUpperTriangular_falseForNearZeroDiagonalEntry(void) {
  Matrix* a = matrix_new(2, 2);
  double vals[] = {
    0.0000001, 5,
    0, 1,
  };
  matrix_set(vals, 2, 2, a);

  bool ok = matrix_isUpperTriangular(a, 1e-3) == false;

  matrix_free(&a);
  return ok;
}

bool test_isUpperTriangular_diagonalExactlyAtErrorFails(void) {
  Matrix* a = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, 0.5);

  bool ok = matrix_isUpperTriangular(a, 0.5) == false;

  matrix_free(&a);
  return ok;
}

bool test_isUpperTriangular_offDiagonalExactlyAtErrorPasses(void) {
  Matrix* a = matrix_new(3, 3);
  matrix_setIdentity(a);
  matrix_setAt(a, 2, 1, 0.5); // abaixo da diagonal, coluna 1 (não 0)

  bool ok = matrix_isUpperTriangular(a, 0.5);

  matrix_free(&a);
  return ok;
}
/* END: testing matrix_isUpperTriangular */

/* BEGIN: testing matrix_solveUpperTriangular
   Fixtures escolhidas pra que a solução X seja inteira e a
   substituição não produza frações, facilitando a checagem exata.
*/
bool test_solveUpperTriangular_basic(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    2, 1, 1,
    0, 3, 1,
    0, 0, 4,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {6, 8, 8}; // = A * [1,2,2]
  matrix_set(bvals, 3, 1, b);
  Matrix* x = matrix_new(3, 1);

  matrix_solveUpperTriangular(a, x, b);
  double expected[] = {1, 2, 2};
  bool ok = expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return ok;
}

// caso 1x1: só divide B[0][0] por A[0][0], sem entrar no loop de
// volta (n = 0, k = n-1 = -1 já falha a condição do while).
bool test_solveUpperTriangular_oneByOne(void) {
  Matrix* a = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, 4.0);
  Matrix* b = matrix_new(1, 1);
  matrix_setAt(b, 0, 0, 10.0);
  Matrix* x = matrix_new(1, 1);

  matrix_solveUpperTriangular(a, x, b);
  double expected[] = {2.5};
  bool ok = expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return ok;
}
/* END: testing matrix_solveUpperTriangular */

/* BEGIN: testing matrix_solveLowerTriangular */
bool test_solveLowerTriangular_basic(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    2, 0, 0,
    1, 3, 0,
    1, 1, 4,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {2, 7, 11}; // = A * [1,2,2]
  matrix_set(bvals, 3, 1, b);
  Matrix* x = matrix_new(3, 1);

  matrix_solveLowerTriangular(a, x, b);
  double expected[] = {1, 2, 2};
  bool ok = expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return ok;
}

bool test_solveLowerTriangular_oneByOne(void) {
  Matrix* a = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, -2.0);
  Matrix* b = matrix_new(1, 1);
  matrix_setAt(b, 0, 0, 5.0);
  Matrix* x = matrix_new(1, 1);

  matrix_solveLowerTriangular(a, x, b);
  double expected[] = {-2.5};
  bool ok = expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return ok;
}
/* END: testing matrix_solveLowerTriangular */

/* BEGIN: testing matrix_solveBySubstitution */
bool test_solveBySubstitution_dispatchesToUpper(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    2, 1, 1,
    0, 3, 1,
    0, 0, 4,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {6, 8, 8};
  matrix_set(bvals, 3, 1, b);
  Matrix* x = matrix_new(3, 1);

  bool result = matrix_solveBySubstitution(a, x, b, 1e-9);
  double expected[] = {1, 2, 2};
  bool ok = result && expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return ok;
}

bool test_solveBySubstitution_dispatchesToLower(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    2, 0, 0,
    1, 3, 0,
    1, 1, 4,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {2, 7, 11};
  matrix_set(bvals, 3, 1, b);
  Matrix* x = matrix_new(3, 1);

  bool result = matrix_solveBySubstitution(a, x, b, 1e-9);
  double expected[] = {1, 2, 2};
  bool ok = result && expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return ok;
}

// matriz diagonal é ao mesmo tempo triangular superior e inferior;
// como a função checa isUpperTriangular primeiro, esse é o ramo que
// deve ser tomado (documenta a ordem de precedência).
bool test_solveBySubstitution_diagonalTakesUpperBranch(void) {
  Matrix* a = matrix_new(2, 2);
  matrix_setIdentity(a);
  matrix_setAt(a, 0, 0, 2.0);
  matrix_setAt(a, 1, 1, 5.0);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {4, 15};
  matrix_set(bvals, 2, 1, b);
  Matrix* x = matrix_new(2, 1);

  bool result = matrix_solveBySubstitution(a, x, b, 1e-9);
  double expected[] = {2, 3};
  bool ok = result && expectMatrix(x, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return ok;
}

// matriz que não é nem triangular superior nem inferior: retorna
// false.
bool test_solveBySubstitution_falseForNonTriangular(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    1, 2,
    3, 4,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {1, 1};
  matrix_set(bvals, 2, 1, b);
  Matrix* x = matrix_new(2, 1);

  bool result = matrix_solveBySubstitution(a, x, b, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&x);
  return result == false;
}
/* END: testing matrix_solveBySubstitution */

/* BEGIN: testing matrix_verifySolution */
bool test_verifySolution_trueForCorrectSolution(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    2, 1, 1,
    0, 3, 1,
    0, 0, 4,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* x = matrix_new(3, 1);
  double xvals[] = {1, 2, 2};
  matrix_set(xvals, 3, 1, x);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {6, 8, 8};
  matrix_set(bvals, 3, 1, b);

  bool ok = matrix_verifySolution(a, x, b, 1e-9);

  matrix_free(&a);
  matrix_free(&x);
  matrix_free(&b);
  return ok;
}

bool test_verifySolution_falseForIncorrectSolution(void) {
  Matrix* a = matrix_new(2, 2);
  matrix_setIdentity(a);
  Matrix* x = matrix_new(2, 1);
  double xvals[] = {1, 1};
  matrix_set(xvals, 2, 1, x);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {1, 5}; // A*X = [1,1], não bate com B
  matrix_set(bvals, 2, 1, b);

  bool ok = matrix_verifySolution(a, x, b, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&x);
  matrix_free(&b);
  return ok;
}

// diferença exatamente igual ao erro ainda é aceita, mesma semântica
// de matrix_equals (usado internamente).
bool test_verifySolution_toleranceBoundaryPasses(void) {
  Matrix* a = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, 1.0);
  Matrix* x = matrix_new(1, 1);
  matrix_setAt(x, 0, 0, 1.0);
  Matrix* b = matrix_new(1, 1);
  matrix_setAt(b, 0, 0, 1.5); // A*X = 1, diferença de 0.5

  bool ok = matrix_verifySolution(a, x, b, 0.5);

  matrix_free(&a);
  matrix_free(&x);
  matrix_free(&b);
  return ok;
}
/* END: testing matrix_verifySolution */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_isLowerTriangular_trueForProperLowerTriangular", test_isLowerTriangular_trueForProperLowerTriangular},
  {"test_isLowerTriangular_falseForNonzeroAboveDiagonal", test_isLowerTriangular_falseForNonzeroAboveDiagonal},
  {"test_isLowerTriangular_falseForNearZeroDiagonalEntry", test_isLowerTriangular_falseForNearZeroDiagonalEntry},
  {"test_isLowerTriangular_diagonalExactlyAtErrorFails", test_isLowerTriangular_diagonalExactlyAtErrorFails},
  {"test_isLowerTriangular_offDiagonalExactlyAtErrorPasses", test_isLowerTriangular_offDiagonalExactlyAtErrorPasses},

  {"test_isUpperTriangular_trueForProperUpperTriangular", test_isUpperTriangular_trueForProperUpperTriangular},
  {"test_isUpperTriangular_falseForNonzeroBelowDiagonal", test_isUpperTriangular_falseForNonzeroBelowDiagonal},
  {"test_isUpperTriangular_falseForNonzeroInFirstColumn", test_isUpperTriangular_falseForNonzeroInFirstColumn},
  {"test_isUpperTriangular_falseForNearZeroDiagonalEntry", test_isUpperTriangular_falseForNearZeroDiagonalEntry},
  {"test_isUpperTriangular_diagonalExactlyAtErrorFails", test_isUpperTriangular_diagonalExactlyAtErrorFails},
  {"test_isUpperTriangular_offDiagonalExactlyAtErrorPasses", test_isUpperTriangular_offDiagonalExactlyAtErrorPasses},

  {"test_solveUpperTriangular_basic", test_solveUpperTriangular_basic},
  {"test_solveUpperTriangular_oneByOne", test_solveUpperTriangular_oneByOne},

  {"test_solveLowerTriangular_basic", test_solveLowerTriangular_basic},
  {"test_solveLowerTriangular_oneByOne", test_solveLowerTriangular_oneByOne},

  {"test_solveBySubstitution_dispatchesToUpper", test_solveBySubstitution_dispatchesToUpper},
  {"test_solveBySubstitution_dispatchesToLower", test_solveBySubstitution_dispatchesToLower},
  {"test_solveBySubstitution_diagonalTakesUpperBranch", test_solveBySubstitution_diagonalTakesUpperBranch},
  {"test_solveBySubstitution_falseForNonTriangular", test_solveBySubstitution_falseForNonTriangular},

  {"test_verifySolution_trueForCorrectSolution", test_verifySolution_trueForCorrectSolution},
  {"test_verifySolution_falseForIncorrectSolution", test_verifySolution_falseForIncorrectSolution},
  {"test_verifySolution_toleranceBoundaryPasses", test_verifySolution_toleranceBoundaryPasses},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("01_subst", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
