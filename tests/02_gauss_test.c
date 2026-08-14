/* Testes unitários pra `02_gauss.h`: i_matrix_findFirstNonNullRow e
   matrix_sqGaussianElimination.
   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 02_gauss_test.c -o out_gausstest
*/

#include "common.h"
#include "../lib/01_subst.h"
#include "../lib/02_gauss.h"

/* BEGIN: testing i_matrix_findFirstNonNullRow */
bool test_findFirstNonNullRow_findsFirstNonzero(void) {
  Matrix* a = matrix_new(4, 1);
  double vals[] = {0, 0, 5, 3};
  matrix_set(vals, 4, 1, a);

  int row = i_matrix_findFirstNonNullRow(a, 0, 0, 1e-9);
  bool ok = row == 2;

  matrix_free(&a);
  return ok;
}

// busca começa em `row_start`, ignorando entradas não-nulas antes
// dele -- é assim que a eliminação evita reusar pivôs já
// processados.
bool test_findFirstNonNullRow_respectsRowStart(void) {
  Matrix* a = matrix_new(4, 1);
  double vals[] = {9, 0, 0, 7};
  matrix_set(vals, 4, 1, a);

  int row = i_matrix_findFirstNonNullRow(a, 1, 0, 1e-9);
  bool ok = row == 3;

  matrix_free(&a);
  return ok;
}

// coluna inteira (a partir de row_start) é zero dentro da
// tolerância: retorna -1, sinalizando matriz não-invertível.
bool test_findFirstNonNullRow_returnsMinusOneWhenAllZero(void) {
  Matrix* a = matrix_new(3, 1);
  double vals[] = {0, 0, 0};
  matrix_set(vals, 3, 1, a);

  int row = i_matrix_findFirstNonNullRow(a, 0, 0, 1e-9);
  bool ok = row == -1;

  matrix_free(&a);
  return ok;
}

// valor exatamente igual ao erro ainda conta como zero (checagem
// usa `> error`, não `>= error`) -- mesma semântica de
// matrix_isLowerTriangular / matrix_isUpperTriangular.
bool test_findFirstNonNullRow_valueExactlyAtErrorCountsAsZero(void) {
  Matrix* a = matrix_new(2, 1);
  double vals[] = {0.5, 3};
  matrix_set(vals, 2, 1, a);

  int row = i_matrix_findFirstNonNullRow(a, 0, 0, 0.5);
  bool ok = row == 1; // linha 0 (valor 0.5) é ignorada, só a 1 conta

  matrix_free(&a);
  return ok;
}

// só olha a coluna pedida, não a linha inteira.
bool test_findFirstNonNullRow_onlyChecksGivenColumn(void) {
  Matrix* a = matrix_new(2, 2);
  double vals[] = {
    0, 8, // coluna 0 é zero nas duas linhas, coluna 1 não
    0, 8,
  };
  matrix_set(vals, 2, 2, a);

  int row = i_matrix_findFirstNonNullRow(a, 0, 0, 1e-9);
  bool ok = row == -1;

  matrix_free(&a);
  return ok;
}
/* END: testing i_matrix_findFirstNonNullRow */

/* BEGIN: testing matrix_sqGaussianElimination */

// 2x2 sem necessidade de pivoteamento (pivô já é não-nulo).
// Cálculo à mão: multiple = -(1/2) = -0.5;
// row1 <- row1 + (-0.5)*row0 = [1-1, 3-0.5] = [0, 2.5];
// B1 <- 10 + (-0.5)*5 = 7.5.
bool test_sqGaussianElimination_noPivotingNeeded(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    2, 1,
    1, 3,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {5, 10};
  matrix_set(bvals, 2, 1, b);

  bool eliminated = matrix_sqGaussianElimination(a, b, 1e-9);

  double expectedA[] = {
    2, 1,
    0, 2.5,
  };
  double expectedB[] = {5, 7.5};
  bool ok = eliminated &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedB, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// pivô nulo na primeira linha força uma troca de linhas; a linha
// trocada já chega com zero na coluna eliminada, então depois do
// swap não sobra nada pra eliminar (multiple = 0).
bool test_sqGaussianElimination_pivotsOnZeroDiagonal(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    0, 2,
    3, 1,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {4, 7};
  matrix_set(bvals, 2, 1, b);

  bool eliminated = matrix_sqGaussianElimination(a, b, 1e-9);

  double expectedA[] = {
    3, 1,
    0, 2,
  };
  double expectedB[] = {7, 4};
  bool ok = eliminated &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedB, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// 3x3 com troca de linha no primeiro passo e dois passos de
// eliminação -- valores conferidos à mão:
//   swap(0,1): A = [[1,0,1],[0,1,1],[1,1,0]], B = [2,2,2]
//   k=0, i=2: multiple=-1, row2 <- row2 + (-1)*row0 = [0,1,-1], B2=0
//   k=1, i=2: multiple=-1, row2 <- row2 + (-1)*row1 = [0,0,-2], B2=-2
bool test_sqGaussianElimination_threeByThreeWithSwap(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    0, 1, 1,
    1, 0, 1,
    1, 1, 0,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {2, 2, 2};
  matrix_set(bvals, 3, 1, b);

  bool eliminated = matrix_sqGaussianElimination(a, b, 1e-9);
  bool upper = matrix_isUpperTriangular(a, 1e-9);

  double expectedA[] = {
    1, 0, 1,
    0, 1, 1,
    0, 0, -2,
  };
  double expectedB[] = {2, 2, -2};
  bool ok = eliminated && upper &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedB, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// matriz singular (linha 1 é múltipla da linha 0): depois de
// eliminar a coluna 0, a linha 1 fica inteiramente zero, e a busca
// por próximo pivô falha -> retorna false.
bool test_sqGaussianElimination_falseForSingularMatrix(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    1, 2,
    2, 4,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {1, 1};
  matrix_set(bvals, 2, 1, b);

  bool ok = matrix_sqGaussianElimination(a, b, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// integração: elimina, resolve por substituição, e confere a
// solução contra o sistema ORIGINAL (antes da eliminação) -- mesmo
// fluxo que os exemplos gauss_01/gauss_02. Usa a mesma matriz de
// exemplos/gauss_01.c.
bool test_sqGaussianElimination_endToEndSolveAndVerify(void) {
  Matrix* a = matrix_new(4, 4);
  double avals[] = {
    1.0, 3.0, 2.0, 1,
      0, 3.0,   1, 1,
      0,   0,   1, 0,
     -1,   0,   0, 4,
  };
  matrix_set(avals, 4, 4, a);
  Matrix* b = matrix_new(4, 1);
  double bvals[] = {1.0, 2.0, 0, 3.0};
  matrix_set(bvals, 4, 1, b);

  Matrix* aOrig = matrix_newCopy(a);
  Matrix* bOrig = matrix_newCopy(b);
  Matrix* x = matrix_new(4, 1);

  bool eliminated = matrix_sqGaussianElimination(a, b, 1e-9);
  bool upper = eliminated && matrix_isUpperTriangular(a, 1e-9);
  bool solved = upper && matrix_solveBySubstitution(a, x, b, 1e-9);
  bool verified = solved && matrix_verifySolution(aOrig, x, bOrig, 1e-9);

  bool ok = eliminated && upper && solved && verified;

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&aOrig);
  matrix_free(&bOrig);
  matrix_free(&x);
  return ok;
}
/* END: testing matrix_sqGaussianElimination */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_findFirstNonNullRow_findsFirstNonzero", test_findFirstNonNullRow_findsFirstNonzero},
  {"test_findFirstNonNullRow_respectsRowStart", test_findFirstNonNullRow_respectsRowStart},
  {"test_findFirstNonNullRow_returnsMinusOneWhenAllZero", test_findFirstNonNullRow_returnsMinusOneWhenAllZero},
  {"test_findFirstNonNullRow_valueExactlyAtErrorCountsAsZero", test_findFirstNonNullRow_valueExactlyAtErrorCountsAsZero},
  {"test_findFirstNonNullRow_onlyChecksGivenColumn", test_findFirstNonNullRow_onlyChecksGivenColumn},

  {"test_sqGaussianElimination_noPivotingNeeded", test_sqGaussianElimination_noPivotingNeeded},
  {"test_sqGaussianElimination_pivotsOnZeroDiagonal", test_sqGaussianElimination_pivotsOnZeroDiagonal},
  {"test_sqGaussianElimination_threeByThreeWithSwap", test_sqGaussianElimination_threeByThreeWithSwap},
  {"test_sqGaussianElimination_falseForSingularMatrix", test_sqGaussianElimination_falseForSingularMatrix},
  {"test_sqGaussianElimination_endToEndSolveAndVerify", test_sqGaussianElimination_endToEndSolveAndVerify},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("02_gauss", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
