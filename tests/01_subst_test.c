/* Testes unitários pra `01_subst.h` (matrix_isLowerTriangular /
   matrix_isUpperTriangular).
   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 01_subst_test.c -o out_substtest

   ATENÇÃO: test_isUpperTriangular_falseForNonzeroInFirstColumn
   documenta um bug real em matrix_isUpperTriangular (ver comentário
   no próprio teste) e por isso FALHA contra a implementação atual
   de lib/01_subst.h. Não é um teste malformado -- é o comportamento
   correto sendo verificado contra uma implementação incorreta.
*/

#include "common.h"
#include "../lib/01_subst.h"

/* NOTA: as UNSAFE-preconditions de 01_subst.h (error < 0) não são
   testadas aqui. Cada uma tem seu próprio programa em tests/crash/,
   que dispara o assert de propósito e espera o processo inteiro
   morrer com SIGABRT -- ver tests/crash/m2_subst_*_crash.c. */

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

// BUG em lib/01_subst.h: o loop interno de matrix_isUpperTriangular
// é `j = i - 1; while (j > 0) { ...; j--; }`, que nunca testa
// j == 0. Ou seja, a coluna 0 abaixo da diagonal nunca é checada, e
// qualquer lixo em A[i][0] (i > 0) passa despercebido.
// Esse teste verifica o comportamento CORRETO (matriz com entrada
// não-nula em A[1][0] não é triangular superior) e por isso falha
// contra a implementação atual -- é esperado até a função ser
// corrigida (o loop deveria ir até `j >= 0`, não `j > 0`).
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

// mesmo cuidado do teste equivalente em isLowerTriangular, mas
// usando coluna 1 (não coluna 0) pra não pisar no bug documentado
// acima e testar só o limite de tolerância em si.
bool test_isUpperTriangular_offDiagonalExactlyAtErrorPasses(void) {
  Matrix* a = matrix_new(3, 3);
  matrix_setIdentity(a);
  matrix_setAt(a, 2, 1, 0.5); // abaixo da diagonal, coluna 1 (não 0)

  bool ok = matrix_isUpperTriangular(a, 0.5);

  matrix_free(&a);
  return ok;
}
/* END: testing matrix_isUpperTriangular */

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
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("01_subst", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
