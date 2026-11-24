#include "../lib/matrix.h"
#include "../lib/15_wave.h"

#include <math.h>
#include <stdio.h>

/* Mesma simulação de wave_05.c (refletor parabólico), mas em vez de imprimir
   quadros guarda, em cada nó da malha, o máximo de |u| ao longo do tempo.
   Saída: matriz N x N (linha i = x_i, coluna j = y_j). */

#define C      1.0
#define N      201
#define H      (1.0 / (N - 1))
#define T_END  1.2

#define WAVELENGTH 0.05
#define SIGMA      0.04
#define PACKET_X   0.18
#define BEAM_WIDTH 0.4

#define CENTER_Y             0.5
#define MIRROR_VERTEX_X      0.92
#define MIRROR_FOCAL         0.3
#define MIRROR_HALF_APERTURE 0.3
#define MIRROR_THICKNESS     0.015

#define PI 3.14159265358979323846

double mirror(double x, double y) {
  double dy = y - CENTER_Y;
  if (fabs(dy) > MIRROR_HALF_APERTURE) {
    return 0;
  }
  double xCurve = MIRROR_VERTEX_X - dy*dy / (4*MIRROR_FOCAL);
  double slope = dy / (2*MIRROR_FOCAL);
  double distance = fabs(x - xCurve) / sqrt(1 + slope*slope);
  return distance < MIRROR_THICKNESS / 2;
}

double initial(double x, double y) {
  double k = 2 * PI / WAVELENGTH;
  double dx = x - PACKET_X;
  double envelopeY = exp(-pow((y - CENTER_Y) / BEAM_WIDTH, 6));
  return exp(-dx*dx / (2*SIGMA*SIGMA)) * cos(k*dx) * envelopeY;
}

static Matrix* g_max = NULL;

void trackMax(Matrix* u) {
  int i = 0;
  while (i < u->rows) {
    int j = 0;
    while (j < u->columns) {
      double a = fabs(matrix_at(u, i, j));
      if (a > matrix_at(g_max, i, j)) {
        matrix_setAt(g_max, i, j, a);
      }
      j++;
    }
    i++;
  }
}

int main(void) {
  const double dt = 0.9 * H / (C * sqrt(2.0));
  g_max = matrix_new(N, N);
  if (g_max == NULL) {
    return 1;
  }
  de_solveWave(initial, mirror, C, trackMax, H, dt, T_END);

  int i = 0;
  while (i < N) {
    int j = 0;
    while (j < N) {
      printf("%.8g\t", matrix_at(g_max, i, j));
      j++;
    }
    printf("\n");
    i++;
  }
  matrix_free(&g_max);
  return 0;
}
