#include "../lib/matrix.h"
#include "../lib/11_explicitParabolic.h"

#include <math.h>
#include <stdio.h>

#define X_A 0.0
#define X_B 1.0
#define N_X 11
#define N_T 21

double f(double t) { return exp(-t) + t; }
double g(double t) { return exp(-t) * cos(1.0) + t + 0.5; }
double w(double x) { return cos(x) + 0.5 * x * x; }

static Matrix* g_collected = NULL;
static int g_calls = 0;

void collect(Matrix* m) {
  if (g_calls < N_T) {
    int i = 0;
    while (i < m->rows) {
      matrix_setAt(g_collected, i, g_calls, matrix_at(m, i, 0));
      i++;
    }
  }
  g_calls++;
}

int main(void) {
  Matrix* reference = de_solveParabolic(f, g, w, X_A, X_B, N_X, N_T);
  g_collected = matrix_new(N_X, N_T);
  if (reference == NULL || g_collected == NULL) {
    fprintf(stderr, "falha ao alocar as matrizes\n");
    return 1;
  }

  bool solved = de_solveParabolicInPlace(f, g, w, X_A, X_B, N_X, N_T, collect);

  double maxDiff = 0.0;
  int j = 0;
  while (j < N_T) {
    int i = 0;
    while (i < N_X) {
      double d = fabs(matrix_at(reference, i, j) - matrix_at(g_collected, i, j));
      /* escrito assim para que NaN propague em vez de ser ignorado */
      if (!(d <= maxDiff)) {
        maxDiff = d;
      }
      i++;
    }
    j++;
  }

  printf("chamadas = %d (esperado %d), maior |dif| = %.3e\n",
         g_calls, N_T, maxDiff);

  matrix_free(&reference);
  matrix_free(&g_collected);

  if (solved && g_calls == N_T && maxDiff == 0.0) {
    printf("solução correta!\n");
    return 0;
  }
  printf("solução incorreta!\n");
  return 1;
}
