/* Implementa trocentos testes unitários pra `matrix.h`.
   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 matrix_test.c -o out_matrixtest

   Esse arquivo foi originalmente gerado pelo Claude Sonnet 5 copiando o
   estilo do repositório github.com/padeir0/pao, e depois refatorado pra
   usar common.h e as funções de matrix.h (matrix_setAt, matrix_set,
   matrix_setIdentity, matrix_equals) em vez de reimplementá-las.
*/

#include "common.h"

/* NOTA: as UNSAFE-preconditions de matrix.h (dimensões inválidas,
   formas incompatíveis, aliasing proibido, etc.) não são testadas
   aqui. Cada uma tem seu próprio programa em tests/crash/, que
   dispara o assert de propósito e espera o processo inteiro morrer
   com SIGABRT -- ver tests/crash/m2_matrix_*_crash.c. */

/* BEGIN: testing matrix_at / matrix_setAt */
bool test_at_setAt_roundTrip(void) {
  Matrix* m = matrix_new(2, 3);
  matrix_setAt(m, 0, 0, 1.5);
  matrix_setAt(m, 0, 2, -3.0);
  matrix_setAt(m, 1, 1, 42.0);

  bool ok = closeEnough(matrix_at(m, 0, 0), 1.5, 1e-9) &&
            closeEnough(matrix_at(m, 0, 2), -3.0, 1e-9) &&
            closeEnough(matrix_at(m, 1, 1), 42.0, 1e-9) &&
            closeEnough(matrix_at(m, 0, 1), 0.0, 1e-9); // não tocada, ainda zero

  matrix_free(&m);
  return ok;
}
/* END: testing matrix_at / matrix_setAt */

/* BEGIN: testing matrix_length */
bool test_length_basic(void) {
  Matrix* m = matrix_new(3, 4);
  bool ok = matrix_length(m) == 12;
  matrix_free(&m);
  return ok;
}
/* END: testing matrix_length */

/* BEGIN: testing matrix_sameShape */
bool test_sameShape_trueForEqualDimensions(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* b = matrix_new(2, 3);
  bool ok = matrix_sameShape(a, b) != 0;
  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

bool test_sameShape_falseForDifferentDimensions(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* b = matrix_new(3, 2);
  bool ok = matrix_sameShape(a, b) == 0;
  matrix_free(&a);
  matrix_free(&b);
  return ok;
}
/* END: testing matrix_sameShape */

/* BEGIN: testing matrix_new / matrix_free */
bool test_new_zeroInitialized(void) {
  Matrix* m = matrix_new(2, 3);
  double expected[] = {0, 0, 0, 0, 0, 0};
  bool ok = m != NULL && m->rows == 2 && m->columns == 3 &&
            expectMatrix(m, expected, 1e-9);
  matrix_free(&m);
  return ok;
}

bool test_free_setsNullAndIsIdempotent(void) {
  Matrix* m = matrix_new(1, 1);
  matrix_free(&m);
  bool ok = m == NULL;
  matrix_free(&m); // no-op, não deve crashar
  return ok;
}
/* END: testing matrix_new / matrix_free */

/* BEGIN: testing matrix_set */
bool test_set_copiesDataWithoutResizing(void) {
  Matrix* m = matrix_new(2, 2);
  double vals[] = {1, 2, 3, 4};

  bool result = matrix_set(vals, 2, 2, m);
  double expected[] = {1, 2, 3, 4};
  bool ok = result && expectMatrix(m, expected, 1e-9);

  matrix_free(&m);
  return ok;
}

// matrix_set também é usado pra crescer/encolher a matriz -- o
// buffer é realocado e rows/columns atualizados.
bool test_set_growsAndUpdatesShape(void) {
  Matrix* m = matrix_new(2, 2);
  double vals[] = {1, 2, 3, 4, 5, 6};

  bool result = matrix_set(vals, 2, 3, m);
  double expected[] = {1, 2, 3, 4, 5, 6};
  bool ok = result && m->rows == 2 && m->columns == 3 &&
            expectMatrix(m, expected, 1e-9);

  matrix_free(&m);
  return ok;
}

bool test_set_shrinksAndUpdatesShape(void) {
  Matrix* m = matrix_new(3, 3);
  fillMatrix(m, 1.0);
  double vals[] = {9, 8};

  bool result = matrix_set(vals, 1, 2, m);
  double expected[] = {9, 8};
  bool ok = result && m->rows == 1 && m->columns == 2 &&
            expectMatrix(m, expected, 1e-9);

  matrix_free(&m);
  return ok;
}
/* END: testing matrix_set */

/* BEGIN: testing matrix_setIdentity */
bool test_setIdentity_basic(void) {
  Matrix* m = matrix_new(3, 3);
  fillMatrix(m, 1.0); // lixo, pra garantir que setIdentity sobrescreve tudo

  matrix_setIdentity(m);
  double expected[] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
  };
  bool ok = expectMatrix(m, expected, 1e-9);

  matrix_free(&m);
  return ok;
}
/* END: testing matrix_setIdentity */

/* BEGIN: testing matrix_equals */
bool test_equals_trueForIdentical(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* b = matrix_new(2, 2);
  fillMatrix(a, 1.0);
  fillMatrix(b, 1.0);

  bool ok = matrix_equals(a, b, 0.0);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

bool test_equals_falseForDifferentShape(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* b = matrix_new(3, 2);

  bool ok = matrix_equals(a, b, 1e9) == false; // erro gigante não importa, forma já difere

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// diferença exatamente igual ao erro ainda é aceita (comparação é <=).
bool test_equals_toleranceBoundaryPasses(void) {
  Matrix* a = matrix_new(1, 1);
  Matrix* b = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, 1.0);
  matrix_setAt(b, 0, 0, 1.5);

  bool ok = matrix_equals(a, b, 0.5);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// diferença um pouco maior que o erro já reprova.
bool test_equals_justAboveToleranceFails(void) {
  Matrix* a = matrix_new(1, 1);
  Matrix* b = matrix_new(1, 1);
  matrix_setAt(a, 0, 0, 1.0);
  matrix_setAt(b, 0, 0, 1.50001);

  bool ok = matrix_equals(a, b, 0.5) == false;

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}
/* END: testing matrix_equals */

/* BEGIN: testing matrix_add */
bool test_add_basic(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* b = matrix_new(2, 3);
  Matrix* c = matrix_new(2, 3);
  fillMatrix(a, 1.0);  // [[1,2,3],[4,5,6]]
  fillMatrix(b, 10.0); // [[10,11,12],[13,14,15]]

  matrix_add(a, b, c);
  double expected[] = {11, 13, 15, 17, 19, 21};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  return ok;
}

bool test_add_zeroIsIdentity(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* zero = matrix_new(2, 2); // zero-initialized por matrix_new
  Matrix* c = matrix_new(2, 2);
  fillMatrix(a, -3.5);

  matrix_add(a, zero, c);
  bool ok = matrix_equals(c, a, 1e-9);

  matrix_free(&a);
  matrix_free(&zero);
  matrix_free(&c);
  return ok;
}

// aliasing total: add(A, A, A) dobra cada célula.
bool test_add_aliasFullyAliased(void) {
  Matrix* a = matrix_new(2, 3);
  fillMatrix(a, 1.0); // [[1,2,3],[4,5,6]]

  matrix_add(a, a, a);
  double expected[] = {2, 4, 6, 8, 10, 12};
  bool ok = expectMatrix(a, expected, 1e-9);

  matrix_free(&a);
  return ok;
}

// aliasing parcial: C == A, B é outra matriz.
bool test_add_aliasOutEqualsFirstOperand(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* b = matrix_new(2, 2);
  fillMatrix(a, 1.0); // [[1,2],[3,4]]
  fillMatrix(b, 10.0); // [[10,11],[12,13]]

  matrix_add(a, b, a);
  double expected[] = {11, 13, 15, 17};
  bool ok = expectMatrix(a, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}
/* END: testing matrix_add */

/* BEGIN: testing matrix_sub */
bool test_sub_basic(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* b = matrix_new(2, 3);
  Matrix* c = matrix_new(2, 3);
  fillMatrix(a, 1.0);  // [[1,2,3],[4,5,6]]
  fillMatrix(b, 10.0); // [[10,11,12],[13,14,15]]

  matrix_sub(b, a, c);
  double expected[] = {9, 9, 9, 9, 9, 9};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  return ok;
}

// aliasing total: sub(A, A, A) = matriz nula.
bool test_sub_aliasFullyAliased(void) {
  Matrix* a = matrix_new(2, 2);
  fillMatrix(a, 3.5);

  matrix_sub(a, a, a);
  double expected[] = {0, 0, 0, 0};
  bool ok = expectMatrix(a, expected, 1e-9);

  matrix_free(&a);
  return ok;
}

// C == A (primeiro operando): a subtração não é comutativa, então
// esse caso testa que a leitura de A e B acontece antes da escrita
// em C, mesmo quando C e A compartilham o buffer.
bool test_sub_aliasOutEqualsFirstOperand(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* b = matrix_new(2, 2);
  fillMatrix(a, 10.0); // [[10,11],[12,13]]
  fillMatrix(b, 1.0);  // [[1,2],[3,4]]

  matrix_sub(a, b, a);
  double expected[] = {9, 9, 9, 9};
  bool ok = expectMatrix(a, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// C == B (segundo operando): mesma preocupação que o teste acima,
// mas do lado que costuma ser esquecido -- garante que não vira
// B - A por engano.
bool test_sub_aliasOutEqualsSecondOperand(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* b = matrix_new(2, 2);
  fillMatrix(a, 10.0); // [[10,11],[12,13]]
  fillMatrix(b, 1.0);  // [[1,2],[3,4]]

  matrix_sub(a, b, b);
  double expected[] = {9, 9, 9, 9};
  bool ok = expectMatrix(b, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}
/* END: testing matrix_sub */

/* BEGIN: testing matrix_scalarMult */
bool test_scalarMult_basic(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* c = matrix_new(2, 3);
  fillMatrix(a, 1.0); // [[1,2,3],[4,5,6]]

  matrix_scalarMult(a, 2.0, c);
  double expected[] = {2, 4, 6, 8, 10, 12};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&c);
  return ok;
}

bool test_scalarMult_byZero(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* c = matrix_new(2, 2);
  fillMatrix(a, 42.0);

  matrix_scalarMult(a, 0.0, c);
  double expected[] = {0, 0, 0, 0};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&c);
  return ok;
}

// aliasing: C == A.
bool test_scalarMult_aliased(void) {
  Matrix* a = matrix_new(2, 2);
  fillMatrix(a, 1.0); // [[1,2],[3,4]]

  matrix_scalarMult(a, 3.0, a);
  double expected[] = {3, 6, 9, 12};
  bool ok = expectMatrix(a, expected, 1e-9);

  matrix_free(&a);
  return ok;
}
/* END: testing matrix_scalarMult */

/* BEGIN: testing matrix_mult */
bool test_mult_basic(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* aT = matrix_new(3, 2);
  Matrix* c = matrix_new(2, 2);
  fillMatrix(a, 1.0); // [[1,2,3],[4,5,6]]
  matrix_transpose(a, aT);

  matrix_mult(a, aT, c); // a * a^T
  double expected[] = {14, 32, 32, 77};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&aT);
  matrix_free(&c);
  return ok;
}

bool test_mult_identity(void) {
  Matrix* a = matrix_new(3, 3);
  Matrix* id = matrix_new(3, 3);
  Matrix* c = matrix_new(3, 3);
  fillMatrix(a, 1.0);
  matrix_setIdentity(id);

  matrix_mult(a, id, c);
  bool ok = matrix_equals(c, a, 1e-9);

  matrix_free(&a);
  matrix_free(&id);
  matrix_free(&c);
  return ok;
}

// dimensões retangulares distintas nos dois operandos, pra
// exercitar os limites dos três loops além do caso quase-quadrado
// acima.
bool test_mult_rectangularShapes(void) {
  Matrix* a = matrix_new(3, 2);
  Matrix* b = matrix_new(2, 4);
  Matrix* c = matrix_new(3, 4);
  double aVals[] = {1, 2, 3, 4, 5, 6};
  double bVals[] = {1, 0, 0, 1, 0, 1, 1, 0};
  matrix_set(aVals, 3, 2, a);
  matrix_set(bVals, 2, 4, b);

  matrix_mult(a, b, c);
  double expected[] = {
    1, 2, 2, 1,
    3, 4, 4, 3,
    5, 6, 6, 5,
  };
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  return ok;
}
/* END: testing matrix_mult */

/* BEGIN: testing matrix_transpose */
bool test_transpose_basic(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* c = matrix_new(3, 2);
  fillMatrix(a, 1.0); // [[1,2,3],[4,5,6]]

  matrix_transpose(a, c);
  double expected[] = {1, 4, 2, 5, 3, 6};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&c);
  return ok;
}

// (A^T)^T == A.
bool test_transpose_involution(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* aT = matrix_new(3, 2);
  Matrix* aTT = matrix_new(2, 3);
  fillMatrix(a, 1.0);

  matrix_transpose(a, aT);
  matrix_transpose(aT, aTT);
  bool ok = matrix_equals(aTT, a, 1e-9);

  matrix_free(&a);
  matrix_free(&aT);
  matrix_free(&aTT);
  return ok;
}

// aliasing: só é possível no lugar pra matriz quadrada.
bool test_transpose_inPlaceSquare(void) {
  Matrix* a = matrix_new(3, 3);
  fillMatrix(a, 1.0); // [[1,2,3],[4,5,6],[7,8,9]]

  matrix_transpose(a, a);
  double expected[] = {1, 4, 7, 2, 5, 8, 3, 6, 9};
  bool ok = expectMatrix(a, expected, 1e-9);

  matrix_free(&a);
  return ok;
}
/* END: testing matrix_transpose */

/* BEGIN: reuse tests ("grow/shrink" analogue)
   Matrix não tem noção de capacidade crescente feito Natural
   (aloca um buffer de tamanho fixo de uma vez só em matrix_new).
   O equivalente aqui é reaproveitar `C` entre chamadas: como as
   operações não alocam, é responsabilidade delas não deixar resíduo
   do conteúdo anterior de `C`. add/sub/scalarMult escrevem cada
   célula de C incondicionalmente, então são seguras "de graça"; já
   matrix_mult usa `C` como acumulador (+=), então precisa
   zerar C explicitamente -- é exatamente o bug que essas duas
   primeiras testam.
*/
bool test_mult_reuse_overwritesStaleData(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* aT = matrix_new(3, 2);
  Matrix* c = matrix_new(2, 2);
  fillMatrix(a, 1.0);
  matrix_transpose(a, aT);
  fillMatrix(c, 999.0); // lixo bem maior que o resultado esperado

  matrix_mult(a, aT, c);
  double expected[] = {14, 32, 32, 77};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&aT);
  matrix_free(&c);
  return ok;
}

bool test_mult_reuse_acrossDifferentInputs(void) {
  Matrix* a1 = matrix_new(2, 2);
  Matrix* b1 = matrix_new(2, 2);
  Matrix* a2 = matrix_new(2, 2);
  Matrix* b2 = matrix_new(2, 2);
  Matrix* c = matrix_new(2, 2);

  double a1Vals[] = {1, 0, 0, 1};
  double b1Vals[] = {5, 6, 7, 8};
  matrix_set(a1Vals, 2, 2, a1);
  matrix_set(b1Vals, 2, 2, b1);
  matrix_mult(a1, b1, c); // c = b1 (identidade * b1)

  double a2Vals[] = {2, 0, 0, 2};
  double b2Vals[] = {1, 1, 1, 1};
  matrix_set(a2Vals, 2, 2, a2);
  matrix_set(b2Vals, 2, 2, b2);
  matrix_mult(a2, b2, c); // reaproveita c com um resultado bem diferente

  double expected[] = {2, 2, 2, 2};
  bool ok = expectMatrix(c, expected, 1e-9);

  matrix_free(&a1);
  matrix_free(&b1);
  matrix_free(&a2);
  matrix_free(&b2);
  matrix_free(&c);
  return ok;
}
/* END: reuse tests */

/* BEGIN: testing matrix_snprint / matrix_printingSize */
bool test_snprint_basic(void) {
  Matrix* a = matrix_new(2, 2);
  double vals[] = {1.0, -2.5, 10.25, 0.0};
  matrix_set(vals, 2, 2, a);

  size_t needed = matrix_printingSize(a, 2);
  char* buffer = (char*)malloc(needed);
  size_t written = matrix_snprint(a, 2, buffer, needed);

  const char* expected = "[  1.00  -2.50 ]\n[ 10.25   0.00 ]";
  bool ok = written == needed &&
            written == strlen(expected) &&
            memcmp(buffer, expected, written) == 0;

  free(buffer);
  matrix_free(&a);
  return ok;
}

// buffer insuficiente: não escreve nada e retorna 0.
bool test_snprint_bufferTooSmallReturnsZero(void) {
  Matrix* a = matrix_new(2, 2);
  fillMatrix(a, 1.0);

  size_t needed = matrix_printingSize(a, 2);
  char* buffer = (char*)malloc(needed);
  size_t written = matrix_snprint(a, 2, buffer, needed - 1);
  bool ok = written == 0;

  free(buffer);
  matrix_free(&a);
  return ok;
}

// matrix_printingSize deve bater exatamente com o que
// matrix_snprint escreve.
bool test_snprintSize_matchesPrettyPrint(void) {
  Matrix* a = matrix_new(3, 4);
  fillMatrix(a, -5.125);

  size_t needed = matrix_printingSize(a, 3);
  char* buffer = (char*)malloc(needed);
  size_t written = matrix_snprint(a, 3, buffer, needed);
  bool ok = written == needed;

  free(buffer);
  matrix_free(&a);
  return ok;
}
/* END: testing matrix_snprint / matrix_printingSize */

/* BEGIN: testing matrix_print
   matrix_print escreve no stdout de verdade, então pra testar sem
   depender de captura externa (como faz `crun`/`test`), redireciona
   fd 1 pra um arquivo temporário, roda a função, e restaura o fd
   original via dup/dup2 antes de comparar o conteúdo do arquivo.
*/
bool test_print_matchesSnprintOutput(void) {
  Matrix* a = matrix_new(2, 2);
  double vals[] = {1.0, -2.5, 10.25, 0.0};
  matrix_set(vals, 2, 2, a);

  char path[64];
  snprintf(path, sizeof(path), "/tmp/m2_matrix_print_test_%d.txt", (int)getpid());

  int savedFd = dup(STDOUT_FILENO);
  bool ok = savedFd != -1;

  if (ok) {
    FILE* redirected = freopen(path, "w", stdout);
    ok = redirected != NULL;
    if (ok) {
      matrix_print(a, 2);
      fflush(stdout);
    }
    dup2(savedFd, STDOUT_FILENO);
    close(savedFd);
  }

  if (ok) {
    FILE* r = fopen(path, "r");
    ok = r != NULL;
    if (ok) {
      char got[256];
      memset(got, 0, sizeof(got));
      size_t n = fread(got, 1, sizeof(got) - 1, r);
      (void)n;
      fclose(r);

      // matrix_print = matrix_snprint + '\n' final
      const char* expected = "[  1.00  -2.50 ]\n[ 10.25   0.00 ]\n";
      ok = strcmp(got, expected) == 0;
      if (!ok) {
        fprintf(stderr, "  test_print: got \"%s\" want \"%s\"\n", got, expected);
      }
    }
  }

  remove(path);
  matrix_free(&a);
  return ok;
}
/* END: testing matrix_print */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_at_setAt_roundTrip", test_at_setAt_roundTrip},

  {"test_length_basic", test_length_basic},

  {"test_sameShape_trueForEqualDimensions", test_sameShape_trueForEqualDimensions},
  {"test_sameShape_falseForDifferentDimensions", test_sameShape_falseForDifferentDimensions},

  {"test_new_zeroInitialized", test_new_zeroInitialized},
  {"test_free_setsNullAndIsIdempotent", test_free_setsNullAndIsIdempotent},

  {"test_set_copiesDataWithoutResizing", test_set_copiesDataWithoutResizing},
  {"test_set_growsAndUpdatesShape", test_set_growsAndUpdatesShape},
  {"test_set_shrinksAndUpdatesShape", test_set_shrinksAndUpdatesShape},

  {"test_setIdentity_basic", test_setIdentity_basic},

  {"test_equals_trueForIdentical", test_equals_trueForIdentical},
  {"test_equals_falseForDifferentShape", test_equals_falseForDifferentShape},
  {"test_equals_toleranceBoundaryPasses", test_equals_toleranceBoundaryPasses},
  {"test_equals_justAboveToleranceFails", test_equals_justAboveToleranceFails},

  {"test_add_basic", test_add_basic},
  {"test_add_zeroIsIdentity", test_add_zeroIsIdentity},
  {"test_add_aliasFullyAliased", test_add_aliasFullyAliased},
  {"test_add_aliasOutEqualsFirstOperand", test_add_aliasOutEqualsFirstOperand},

  {"test_sub_basic", test_sub_basic},
  {"test_sub_aliasFullyAliased", test_sub_aliasFullyAliased},
  {"test_sub_aliasOutEqualsFirstOperand", test_sub_aliasOutEqualsFirstOperand},
  {"test_sub_aliasOutEqualsSecondOperand", test_sub_aliasOutEqualsSecondOperand},

  {"test_scalarMult_basic", test_scalarMult_basic},
  {"test_scalarMult_byZero", test_scalarMult_byZero},
  {"test_scalarMult_aliased", test_scalarMult_aliased},

  {"test_mult_basic", test_mult_basic},
  {"test_mult_identity", test_mult_identity},
  {"test_mult_rectangularShapes", test_mult_rectangularShapes},

  {"test_transpose_basic", test_transpose_basic},
  {"test_transpose_involution", test_transpose_involution},
  {"test_transpose_inPlaceSquare", test_transpose_inPlaceSquare},

  {"test_mult_reuse_overwritesStaleData", test_mult_reuse_overwritesStaleData},
  {"test_mult_reuse_acrossDifferentInputs", test_mult_reuse_acrossDifferentInputs},

  {"test_snprint_basic", test_snprint_basic},
  {"test_snprint_bufferTooSmallReturnsZero", test_snprint_bufferTooSmallReturnsZero},
  {"test_snprintSize_matchesPrettyPrint", test_snprintSize_matchesPrettyPrint},

  {"test_print_matchesSnprintOutput", test_print_matchesSnprintOutput},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("matrix", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
