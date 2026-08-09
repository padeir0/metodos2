/* Infraestrutura compartilhada entre os arquivos de teste (*_test.c)
   desse repositório: harness de execução, fixtures de matriz, e
   comparação com tolerância.

   Cada *_test.c deve incluir esse arquivo (além do header da lib
   que estiver testando) e evitar reimplementar o que já existe em
   `lib/matrix.h` -- por exemplo, usar matrix_setAt/matrix_set/
   matrix_setIdentity/matrix_equals em vez de mexer em `m->data`
   ou nos campos de Matrix diretamente.

   As UNSAFE-preconditions (assert) que esperam abort() NÃO são
   testadas aqui: cada uma vira um programa próprio em tests/crash/
   (um crash por arquivo, sem fork nem estado global), no modelo do
   repositório github.com/padeir0/pao. O script `tests/test` já sabe
   achar e rodar esses arquivos.

   Estilo copiado do repositório github.com/padeir0/pao, com o
   toque de cor que já existia em matrix_test.c.
*/
#include "../lib/matrix.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef M2_TEST_COMMON_H
#define M2_TEST_COMMON_H 1

#define common_RED    "\x1b[0;31m"
#define common_BLUE   "\x1b[0;34m"
#define common_RESET  "\x1b[0m"

/* BEGIN: test runner */

typedef bool (*TestFn)(void);

typedef struct {
  const char* name;
  TestFn fn;
} Tester;

// roda cada teste em `tests`, imprime OK/FAIL por teste e um resumo
// colorido no final. Retorna a quantidade de testes que falharam.
int run_tests(const char* suite, Tester* tests, int n) {
  int failed = 0;
  int i = 0;
  while (i < n) {
    bool ok = tests[i].fn();
    printf("[%s] %-48s %s\n", suite, tests[i].name,
           ok ? "OK" : common_RED "FAIL" common_RESET);
    if (!ok) {
      failed++;
    }
    i++;
  }
  printf(common_BLUE "[%s] %d/%d passed\n" common_RESET, suite, n - failed, n);
  return failed;
}
/* END: test runner */

/* BEGIN: matrix fixtures
   Helpers pra montar matrizes de teste. Usam os acessores de
   matrix.h (matrix_setAt/matrix_set) em vez de indexar `m->data`
   diretamente, pra não reimplementar i_matrix_index aqui.
*/

// preenche `m` com valores sequenciais start, start+1, start+2, ...
// em row-major order. Não existe equivalente em matrix.h (é só uma
// fixture de teste), mas escreve célula a célula via matrix_setAt.
void fillMatrix(Matrix* m, double start) {
  double v = start;
  int i = 0;
  while (i < m->rows) {
    int j = 0;
    while (j < m->columns) {
      matrix_setAt(m, i, j, v);
      v += 1.0;
      j++;
    }
    i++;
  }
}
/* END: matrix fixtures */

/* BEGIN: comparison with tolerance */

bool closeEnough(double got, double want, double error) {
  return fabs(got - want) <= error;
}

// compara `got` contra os valores em `expectedVals` (row-major,
// mesmo tamanho que got->rows*got->columns), reaproveitando
// matrix_equals de matrix.h em vez de comparar célula a célula na
// mão. `expectedVals` não é modificado; o cast em matrix_set é só
// porque a assinatura da lib pede `double*` não-const.
bool expectMatrix(const Matrix* got, const double* expectedVals, double error) {
  Matrix* want = matrix_new(got->rows, got->columns);
  if (want == NULL) {
    fprintf(stderr, "  expectMatrix: out of memory building expected matrix\n");
    return false;
  }
  matrix_set((double*)expectedVals, got->rows, got->columns, want);

  bool ok = matrix_equals(got, want, error);
  if (!ok) {
    int i = 0;
    while (i < got->rows) {
      int j = 0;
      while (j < got->columns) {
        double g = matrix_at(got, i, j);
        double w = matrix_at(want, i, j);
        if (!closeEnough(g, w, error)) {
          fprintf(stderr, "  expectMatrix: cell (%d,%d): got %f want %f\n", i, j, g, w);
        }
        j++;
      }
      i++;
    }
  }

  matrix_free(&want);
  return ok;
}
/* END: comparison with tolerance */

#endif
