/* Implementa trocentos testes unitários pra `matrix.h`.
   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 matrix_test.c -o out_matrixtest

   Esse arquivo foi gerado pelo Claude Sonnet 5 copiando o estilo do repositório github.com/padeir0/pao
*/

#include "../lib/matrix.h"

#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* BEGIN: HELPERS */

static void fillMatrix(Matrix* m, double start) {
  double v = start;
  int i = 0;
  while (i < m->rows) {
    int j = 0;
    while (j < m->columns) {
      m->data[i * m->columns + j] = v;
      v += 1.0;
      j++;
    }
    i++;
  }
}

static void fillFromArray(Matrix* m, const double* vals) {
  int n = m->rows * m->columns;
  int k = 0;
  while (k < n) {
    m->data[k] = vals[k];
    k++;
  }
}

static void fillIdentity(Matrix* m) {
  assert(m->rows == m->columns);
  int i = 0;
  while (i < m->rows) {
    int j = 0;
    while (j < m->columns) {
      m->data[i_matrix_index(m, i, j)] = (i == j) ? 1.0 : 0.0;
      j++;
    }
    i++;
  }
}

static bool closeEnough(double got, double want) {
  return fabs(got - want) <= 1e-9;
}

// compara todas as células de `m` contra `expected` (row-major, mesmo
// tamanho que m->rows*m->columns).
static bool matrixEquals(const Matrix* m, const double* expected) {
  int n = m->rows * m->columns;
  int k = 0;
  while (k < n) {
    if (!closeEnough(m->data[k], expected[k])) {
      fprintf(stderr, "  matrixEquals: cell %d: got %f want %f\n", k, m->data[k], expected[k]);
      return false;
    }
    k++;
  }
  return true;
}

// roda `fn` num processo filho e checa que ele morreu por SIGABRT,
// como esperado das UNSAFE-preconditions protegidas por `assert`
// (só funciona se a suíte for compilada sem -DNDEBUG).
static bool expectAbort(void (*fn)(void)) {
  pid_t pid = fork();
  if (pid == 0) {
    freopen("/dev/null", "w", stderr); // silencia a mensagem do assert
    fn();
    _exit(0); // se chegou aqui, fn() não abortou -- é falha do teste
  }
  int status;
  waitpid(pid, &status, 0);
  return WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT;
}

typedef bool (*TestFn)(void);
typedef struct {
  const char* name;
  TestFn fn;
} Tester;

static int run_tests(const char* suite, Tester* tests, int n) {
  int failed = 0;
  int i = 0;
  while (i < n) {
    bool ok = tests[i].fn();
    printf("[%s] %-48s %s\n", suite, tests[i].name, ok ? "OK" : "FAIL");
    if (!ok) {
      failed++;
    }
    i++;
  }
  printf("\033[0;34m[%s] %d/%d passed \033[0m\n", suite, n - failed, n);
  return failed;
}
/* END: HELPERS */

/* BEGIN: testing matrix_new / matrix_free */
bool test_new_zeroInitialized(void) {
  Matrix* m = matrix_new(2, 3);
  bool ok = m != NULL && m->rows == 2 && m->columns == 3;
  int k = 0;
  while (ok && k < 6) {
    ok = closeEnough(m->data[k], 0.0);
    k++;
  }
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

/* BEGIN: testing matrix_add */
bool test_add_basic(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* b = matrix_new(2, 3);
  Matrix* c = matrix_new(2, 3);
  fillMatrix(a, 1.0);  // [[1,2,3],[4,5,6]]
  fillMatrix(b, 10.0); // [[10,11,12],[13,14,15]]

  matrix_add(a, b, c);
  double expected[] = {11, 13, 15, 17, 19, 21};
  bool ok = matrixEquals(c, expected);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  return ok;
}

bool test_add_zeroIsIdentity(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* zero = matrix_new(2, 2); // zero-initialized by matrix_new
  Matrix* c = matrix_new(2, 2);
  fillMatrix(a, -3.5);

  matrix_add(a, zero, c);
  bool ok = matrixEquals(c, a->data);

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
  bool ok = matrixEquals(a, expected);

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
  bool ok = matrixEquals(a, expected);

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
  bool ok = matrixEquals(c, expected);

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
  bool ok = matrixEquals(a, expected);

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
  bool ok = matrixEquals(a, expected);

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
  bool ok = matrixEquals(b, expected);

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
  bool ok = matrixEquals(c, expected);

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
  bool ok = matrixEquals(c, expected);

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
  bool ok = matrixEquals(a, expected);

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
  bool ok = matrixEquals(c, expected);

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
  fillIdentity(id);

  matrix_mult(a, id, c);
  bool ok = matrixEquals(c, a->data);

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
  fillFromArray(a, aVals);
  fillFromArray(b, bVals);

  matrix_mult(a, b, c);
  double expected[] = {
    1, 2, 2, 1,
    3, 4, 4, 3,
    5, 6, 6, 5,
  };
  bool ok = matrixEquals(c, expected);

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
  bool ok = matrixEquals(c, expected);

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
  bool ok = matrixEquals(aTT, a->data);

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
  bool ok = matrixEquals(a, expected);

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
  bool ok = matrixEquals(c, expected);

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
  fillFromArray(a1, a1Vals);
  fillFromArray(b1, b1Vals);
  matrix_mult(a1, b1, c); // c = b1 (identidade * b1)

  double a2Vals[] = {2, 0, 0, 2};
  double b2Vals[] = {1, 1, 1, 1};
  fillFromArray(a2, a2Vals);
  fillFromArray(b2, b2Vals);
  matrix_mult(a2, b2, c); // reaproveita c com um resultado bem diferente

  double expected[] = {2, 2, 2, 2};
  bool ok = matrixEquals(c, expected);

  matrix_free(&a1);
  matrix_free(&b1);
  matrix_free(&a2);
  matrix_free(&b2);
  matrix_free(&c);
  return ok;
}
/* END: reuse tests */

/* BEGIN: testing the aliasing guards actually abort */
static Matrix* s_abortA;
static Matrix* s_abortB;
static Matrix* s_abortC;

static void doMultAliasedCall(void) {
  matrix_mult(s_abortA, s_abortB, s_abortC);
}

bool test_mult_abortsWhenCAliasesA(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* b = matrix_new(2, 2);
  fillMatrix(a, 1.0);
  fillMatrix(b, 1.0);
  s_abortA = a;
  s_abortB = b;
  s_abortC = a; // C aliases A

  bool ok = expectAbort(doMultAliasedCall);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

static void doTransposeNonSquareAliasedCall(void) {
  matrix_transpose(s_abortA, s_abortA);
}

bool test_transpose_abortsOnNonSquareAliasing(void) {
  Matrix* a = matrix_new(2, 3); // não-quadrada
  fillMatrix(a, 1.0);
  s_abortA = a;

  bool ok = expectAbort(doTransposeNonSquareAliasedCall);

  matrix_free(&a);
  return ok;
}
/* END: testing the aliasing guards actually abort */

/* BEGIN: testing matrix_snprint */
bool test_snprint_basic(void) {
  Matrix* a = matrix_new(2, 2);
  double vals[] = {1.0, -2.5, 10.25, 0.0};
  fillFromArray(a, vals);

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

// matrix_snprintSize deve bater exatamente com o que
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
/* END: testing matrix_snprint */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_new_zeroInitialized", test_new_zeroInitialized},
  {"test_free_setsNullAndIsIdempotent", test_free_setsNullAndIsIdempotent},

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

  {"test_mult_abortsWhenCAliasesA", test_mult_abortsWhenCAliasesA},
  {"test_transpose_abortsOnNonSquareAliasing", test_transpose_abortsOnNonSquareAliasing},

  {"test_snprint_basic", test_snprint_basic},
  {"test_snprint_bufferTooSmallReturnsZero", test_snprint_bufferTooSmallReturnsZero},
  {"test_snprintSize_matchesPrettyPrint", test_snprintSize_matchesPrettyPrint},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
#ifdef NDEBUG
  fprintf(stderr, "esta suite depende de assert() ativo; recompile sem -DNDEBUG\n");
  return 1;
#endif
  int failed = run_tests("matrix", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
