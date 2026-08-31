/* Testes unitários pra `06_lu.h`: matrix_NaiveLUDecomposition,
   matrix_NaiveLDLDecomposition e matrix_PivotingLUDecomposition.

   Valores esperados (L, U, D e as versões mutadas de A) foram conferidos
   simulando o algoritmo com numpy antes de escrever os testes.

   A função interna i_matrix_findLargestRow (idêntica em código à de
   03_gaussPivot.h e 05_gaussJordan.h, mas uma cópia `static` própria
   deste arquivo) não é testada diretamente aqui -- só indiretamente,
   através de matrix_PivotingLUDecomposition.

   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 06_lu_test.c -o out_lutest
*/

#include "common.h"
#include "../lib/01_subst.h"
#include "../lib/06_lu.h"

/* BEGIN: testing matrix_NaiveLUDecomposition */

// A = [[2,1,1],[4,3,3],[8,7,9]], sem necessidade de pivotamento (a função
// não pivota mesmo). L e U conferidos com numpy.
bool test_NaiveLUDecomposition_basic(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    2, 1, 1,
    4, 3, 3,
    8, 7, 9,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* l = matrix_new(3, 3);
  Matrix* u = matrix_new(3, 3);

  bool result = matrix_NaiveLUDecomposition(a, l, u, 1e-9);

  double expectedL[] = {
    1, 0, 0,
    2, 1, 0,
    4, 3, 1,
  };
  double expectedU[] = {
    2, 1, 1,
    0, 1, 1,
    0, 0, 2,
  };

  Matrix* lu = matrix_new(3, 3);
  matrix_mult(l, u, lu);

  bool ok = result &&
            expectMatrix(l, expectedL, 1e-9) &&
            expectMatrix(u, expectedU, 1e-9) &&
            matrix_equals(a, lu, 1e-9); // A = L*U

  matrix_free(&a);
  matrix_free(&l);
  matrix_free(&u);
  matrix_free(&lu);
  return ok;
}

// pivô nulo logo na primeira coluna, sem pivotamento pra contornar ->
// retorna false.
bool test_NaiveLUDecomposition_falseForZeroPivot(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    0, 1,
    1, 0,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* l = matrix_new(2, 2);
  Matrix* u = matrix_new(2, 2);

  bool ok = matrix_NaiveLUDecomposition(a, l, u, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&l);
  matrix_free(&u);
  return ok;
}
/* END: testing matrix_NaiveLUDecomposition */

/* BEGIN: testing matrix_NaiveLDLDecomposition */

// A simétrica = [[4,2,2],[2,5,3],[2,3,6]]. L e D conferidos com numpy;
// A^T=A garante que L*D*L^T reproduz o A ORIGINAL.
bool test_NaiveLDLDecomposition_basic(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    4, 2, 2,
    2, 5, 3,
    2, 3, 6,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* aOrig = matrix_newCopy(a);
  Matrix* l = matrix_new(3, 3);
  Matrix* d = matrix_new(3, 3);

  bool result = matrix_NaiveLDLDecomposition(a, l, d, 1e-9);

  double expectedL[] = {
    1,   0,   0,
    0.5, 1,   0,
    0.5, 0.5, 1,
  };
  double expectedD[] = {
    4, 0, 0,
    0, 4, 0,
    0, 0, 4,
  };

  Matrix* lT = matrix_new(3, 3);
  Matrix* ld = matrix_new(3, 3);
  Matrix* ldlT = matrix_new(3, 3);
  matrix_transpose(l, lT);
  matrix_mult(l, d, ld);
  matrix_mult(ld, lT, ldlT);

  bool ok = result &&
            expectMatrix(l, expectedL, 1e-9) &&
            expectMatrix(d, expectedD, 1e-9) &&
            matrix_equals(aOrig, ldlT, 1e-9); // A_original = L*D*L^T

  matrix_free(&a);
  matrix_free(&aOrig);
  matrix_free(&l);
  matrix_free(&d);
  matrix_free(&lT);
  matrix_free(&ld);
  matrix_free(&ldlT);
  return ok;
}

// diferente de matrix_NaiveLUDecomposition (que usa uma cópia U), essa
// função MODIFICA `A` diretamente -- documenta esse comportamento.
bool test_NaiveLDLDecomposition_modifiesAInPlace(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    4, 2, 2,
    2, 5, 3,
    2, 3, 6,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* l = matrix_new(3, 3);
  Matrix* d = matrix_new(3, 3);

  matrix_NaiveLDLDecomposition(a, l, d, 1e-9);

  double expectedAAfter[] = {
    4, 2, 2,
    0, 4, 2,
    0, 0, 4,
  };
  bool ok = expectMatrix(a, expectedAAfter, 1e-9);

  matrix_free(&a);
  matrix_free(&l);
  matrix_free(&d);
  return ok;
}

// pivô nulo logo de cara -> retorna false (e, nesse caso particular, `A`
// nem chega a ser modificada, pois o teste do pivô vem antes de qualquer
// rowMultAdd).
bool test_NaiveLDLDecomposition_falseForZeroPivot(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    0, 1,
    1, 0,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* l = matrix_new(2, 2);
  Matrix* d = matrix_new(2, 2);

  bool ok = matrix_NaiveLDLDecomposition(a, l, d, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&l);
  matrix_free(&d);
  return ok;
}
/* END: testing matrix_NaiveLDLDecomposition */

/* BEGIN: testing matrix_PivotingLUDecomposition */

// A = [[1,2,3],[4,5,6],[7,8,10]]: o maior módulo da coluna 0 é 7 (linha
// 2), então a função troca linha 0 com a linha 2 logo em k=0. Conferido
// com numpy que P*A == L*U (P[0][2]==1 confirma que a troca aconteceu).
bool test_PivotingLUDecomposition_requiresSwap(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    1, 2, 3,
    4, 5, 6,
    7, 8, 10,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* p = matrix_new(3, 3);
  Matrix* l = matrix_new(3, 3);
  Matrix* u = matrix_new(3, 3);

  bool result = matrix_PivotingLUDecomposition(a, p, l, u, 1e-9);

  Matrix* pa = matrix_new(3, 3);
  Matrix* lu = matrix_new(3, 3);
  matrix_mult(p, a, pa);
  matrix_mult(l, u, lu);

  bool swapHappened = closeEnough(matrix_at(p, 0, 2), 1.0, 1e-9);

  bool ok = result && swapHappened && matrix_equals(pa, lu, 1e-9);

  matrix_free(&a);
  matrix_free(&p);
  matrix_free(&l);
  matrix_free(&u);
  matrix_free(&pa);
  matrix_free(&lu);
  return ok;
}

// A diagonalmente dominante = [[9,1,1],[1,8,1],[1,1,7]]: o maior módulo de
// cada coluna (a partir da linha atual) já está na diagonal em todo passo,
// então P permanece a identidade (nenhuma troca de fato acontece).
bool test_PivotingLUDecomposition_noSwapNeeded(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    9, 1, 1,
    1, 8, 1,
    1, 1, 7,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* p = matrix_new(3, 3);
  Matrix* l = matrix_new(3, 3);
  Matrix* u = matrix_new(3, 3);

  bool result = matrix_PivotingLUDecomposition(a, p, l, u, 1e-9);

  double expectedP[] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
  };

  Matrix* lu = matrix_new(3, 3);
  matrix_mult(l, u, lu);

  bool ok = result &&
            expectMatrix(p, expectedP, 1e-9) &&
            matrix_equals(a, lu, 1e-9); // P=I, então A = L*U diretamente

  matrix_free(&a);
  matrix_free(&p);
  matrix_free(&l);
  matrix_free(&u);
  matrix_free(&lu);
  return ok;
}

// coluna inteira nula: não existe pivô possível -> retorna false.
bool test_PivotingLUDecomposition_falseForSingularMatrix(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    0, 1,
    0, 1,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* p = matrix_new(2, 2);
  Matrix* l = matrix_new(2, 2);
  Matrix* u = matrix_new(2, 2);

  bool ok = matrix_PivotingLUDecomposition(a, p, l, u, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&p);
  matrix_free(&l);
  matrix_free(&u);
  return ok;
}
/* END: testing matrix_PivotingLUDecomposition */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_NaiveLUDecomposition_basic", test_NaiveLUDecomposition_basic},
  {"test_NaiveLUDecomposition_falseForZeroPivot", test_NaiveLUDecomposition_falseForZeroPivot},

  {"test_NaiveLDLDecomposition_basic", test_NaiveLDLDecomposition_basic},
  {"test_NaiveLDLDecomposition_modifiesAInPlace", test_NaiveLDLDecomposition_modifiesAInPlace},
  {"test_NaiveLDLDecomposition_falseForZeroPivot", test_NaiveLDLDecomposition_falseForZeroPivot},

  {"test_PivotingLUDecomposition_requiresSwap", test_PivotingLUDecomposition_requiresSwap},
  {"test_PivotingLUDecomposition_noSwapNeeded", test_PivotingLUDecomposition_noSwapNeeded},
  {"test_PivotingLUDecomposition_falseForSingularMatrix", test_PivotingLUDecomposition_falseForSingularMatrix},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("06_lu", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
