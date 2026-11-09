#include "../lib/matrix.h"
#include "../lib/15_wave.h"

#include <math.h>
#include <stdio.h>

/* Gota gaussiana fora do centro, pra reflexão nas bordas ficar interessante.
   Saída: um cabeçalho e depois um quadro a cada STRIDE passos de tempo, cada
   quadro com N linhas de N valores (u[i][j] em x_i, y_j), separados por uma
   linha em branco. Dá pra animar com animate2d.py. */

#define C      1.0
#define N      81
#define H      (1.0 / (N - 1))
#define T_END  2.0
#define STRIDE 4 // imprime 1 a cada STRIDE passos de tempo

double initial(double x, double y) {
  const double x0 = 0.35;
  const double y0 = 0.50;
  const double sigma = 0.06;
  double dx = x - x0;
  double dy = y - y0;
  return exp(-(dx*dx + dy*dy) / (2*sigma*sigma));
}
double noWall(double x, double y) { (void)x; (void)y; return 0; }

static int g_level = 0;

void printFrame(Matrix* u) {
  if (g_level % STRIDE == 0) {
    int i = 0;
    while (i < u->rows) {
      int j = 0;
      while (j < u->columns) {
        printf("%.5f\t", matrix_at(u, i, j));
        j++;
      }
      printf("\n");
      i++;
    }
    printf("\n");
  }
  g_level++;
}

int main(void) {
  // condição CFL em 2D: C*dt/h <= 1/sqrt(2)
  const double dt = 0.9 * H / (C * sqrt(2.0));

  printf("# n_x = %d, n_y = %d, dt = %.8f, stride = %d, x = %g %g, y = %g %g\n",
         N, N, dt, STRIDE, 0.0, 1.0, 0.0, 1.0);

  de_solveWave(initial, noWall, C, printFrame, H, dt, T_END);
  return 0;
}
