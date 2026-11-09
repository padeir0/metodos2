#include "../lib/matrix.h"
#include "../lib/15_wave.h"

#include <math.h>
#include <stdio.h>

/* Mesmo experimento de wave_03.c, mas com fenda dupla: as duas ondas
   circulares que saem das fendas se somam e formam um padrão de interferência
   (faixas de reforço e de cancelamento). Saída no formato de wave_02.c. */

#define C      1.0
#define N      151
#define H      (1.0 / (N - 1))
#define T_END  1.0
#define STRIDE 4

#define SOURCE_X 0.2
#define SOURCE_Y 0.5
#define SIGMA    0.03

#define WALL_X          0.5
#define WALL_THICKNESS  0.02
#define SLIT_WIDTH      0.05
#define SLIT_SEPARATION 0.3 // distância entre os centros das fendas

double initial(double x, double y) {
  double dx = x - SOURCE_X;
  double dy = y - SOURCE_Y;
  return exp(-(dx*dx + dy*dy) / (2*SIGMA*SIGMA));
}

// uma fenda de largura SLIT_WIDTH centrada em `center`
static int inSlit(double y, double center) {
  return fabs(y - center) < SLIT_WIDTH / 2;
}

// obstáculo: parede vertical em x = WALL_X com duas fendas simétricas em y = 0.5
double wall(double x, double y) {
  if (fabs(x - WALL_X) >= WALL_THICKNESS / 2) {
    return 0;
  }
  int open = inSlit(y, 0.5 - SLIT_SEPARATION/2) || inSlit(y, 0.5 + SLIT_SEPARATION/2);
  return !open;
}

static int g_level = 0;

void printFrame(Matrix* u) {
  if (g_level % STRIDE == 0) {
    int i = 0;
    while (i < u->rows) {
      int j = 0;
      while (j < u->columns) {
        printf("%.3f\t", matrix_at(u, i, j));
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

  de_solveWave(initial, wall, C, printFrame, H, dt, T_END);
  return 0;
}
