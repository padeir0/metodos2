#include "../lib/matrix.h"
#include "../lib/15_wave.h"

#include <math.h>
#include <stdio.h>

/* Modo estacionário (1, 2) da membrana quadrada, que tem solução exata:
     u(x, y, t) = sin(pi x) sin(2 pi y) cos(omega t),  omega = c pi sqrt(5)
   Compara a solução numérica com a exata em todos os quadros. */

#define PI 3.14159265358979323846

#define C     1.0
#define N     41
#define H     (1.0 / (N - 1))
#define T_END 2.0

double initial(double x, double y) { return sin(PI * x) * sin(2 * PI * y); }
double noWall(double x, double y) { (void)x; (void)y; return 0; }
double exact(double x, double y, double t) {
  double omega = C * PI * sqrt(5.0);
  return initial(x, y) * cos(omega * t);
}

static int g_level = 0;
static double g_dt = 0;
static double g_maxError = 0;

void collect(Matrix* u) {
  double t = g_level * g_dt;
  int i = 0;
  while (i < u->rows) {
    int j = 0;
    while (j < u->columns) {
      double error = fabs(matrix_at(u, i, j) - exact(i*H, j*H, t));
      // escrito assim para que NaN propague em vez de ser ignorado
      if (!(error <= g_maxError)) {
        g_maxError = error;
      }
      j++;
    }
    i++;
  }
  g_level++;
}

int main(void) {
  // condição CFL em 2D: C*dt/h <= 1/sqrt(2)
  g_dt = 0.9 * H / (C * sqrt(2.0));

  de_solveWave(initial, noWall, C, collect, H, g_dt, T_END);

  printf("# n_x = %d, n_y = %d, dt = %.6f, quadros = %d\n", N, N, g_dt, g_level);
  printf("# erro maximo = %.6e\n", g_maxError);
  return 0;
}
