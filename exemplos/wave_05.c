#include "../lib/matrix.h"
#include "../lib/15_wave.h"

#include <math.h>
#include <stdio.h>

/* Refletor parabólico: um pacote de ondas plano bate num espelho parabólico
   e a onda refletida converge no foco. Pela propriedade da parábola, todo
   raio paralelo ao eixo reflete passando pelo foco e todos percorrem o mesmo
   caminho até lá (x_v + F - x a partir de uma frente plana em x), então
   chegam ao mesmo tempo e somam. A amplitude no foco é comparada com a de uma
   parede reta no mesmo lugar.

   Como a velocidade inicial é 0, o pacote se divide em duas metades: uma vai
   pra direita (a que interessa) e a outra reflete na borda esquerda. T_END é
   escolhido antes de a segunda metade chegar ao foco.

   Saída no formato de wave_02.c (quadros em stdout, resumo em stderr). */

#define C      1.0
#define N      201
#define H      (1.0 / (N - 1))
#define T_END  1.2
#define STRIDE 10

#define WAVELENGTH 0.05 // comprimento de onda do pacote
#define SIGMA      0.04 // largura do envelope gaussiano em x
#define PACKET_X   0.18 // posição inicial do centro do pacote
#define BEAM_WIDTH 0.4  // semilargura (achatada) do feixe em y

// espelho: x = MIRROR_VERTEX_X - (y - CENTER_Y)^2 / (4 F), com a abertura
// voltada pra esquerda e o foco em (MIRROR_VERTEX_X - MIRROR_FOCAL, CENTER_Y)
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
  // distância até a curva medida na normal (e não só em x), pra espessura
  // ser a mesma nas partes inclinadas
  double slope = dy / (2*MIRROR_FOCAL);
  double distance = fabs(x - xCurve) / sqrt(1 + slope*slope);
  return distance < MIRROR_THICKNESS / 2;
}

// parede reta de mesma abertura no mesmo lugar (referência sem foco)
double flatMirror(double x, double y) {
  return fabs(y - CENTER_Y) <= MIRROR_HALF_APERTURE
      && fabs(x - MIRROR_VERTEX_X) < MIRROR_THICKNESS / 2;
}

// pacote plano: gaussiana * cos(k (x - x0)), achatado em y pra ser ~1 na
// região do espelho e ~0 perto das bordas
double initial(double x, double y) {
  double k = 2 * PI / WAVELENGTH;
  double dx = x - PACKET_X;
  double envelopeY = exp(-pow((y - CENTER_Y) / BEAM_WIDTH, 6));
  return exp(-dx*dx / (2*SIGMA*SIGMA)) * cos(k*dx) * envelopeY;
}

static int g_level = 0;
static double g_dt = 0;
static double g_focusPeak = 0;
static double g_focusTime = 0;

// nó da malha que cai exatamente no foco
static int focusI(void) {
  return (int)lround((MIRROR_VERTEX_X - MIRROR_FOCAL) / H);
}
static int focusJ(void) {
  return (int)lround(CENTER_Y / H);
}

static void trackFocus(const Matrix* u) {
  double a = fabs(matrix_at(u, focusI(), focusJ()));
  if (a > g_focusPeak) {
    g_focusPeak = a;
    g_focusTime = g_level * g_dt;
  }
}

void silent(Matrix* u) {
  trackFocus(u);
  g_level++;
}

void printFrame(Matrix* u) {
  trackFocus(u);
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
  g_dt = 0.9 * H / (C * sqrt(2.0));

  printf("# n_x = %d, n_y = %d, dt = %.8f, stride = %d, x = %g %g, y = %g %g\n",
         N, N, g_dt, STRIDE, 0.0, 1.0, 0.0, 1.0);

  de_solveWave(initial, mirror, C, printFrame, H, g_dt, T_END);
  const double parabolaPeak = g_focusPeak;
  const double parabolaTime = g_focusTime;

  g_level = 0;
  g_focusPeak = 0;
  g_focusTime = 0;
  de_solveWave(initial, flatMirror, C, silent, H, g_dt, T_END);

  const double expectedTime = (MIRROR_VERTEX_X + MIRROR_FOCAL - PACKET_X) / C;
  fprintf(stderr, "# foco em (x, y) = (%g, %g)\n",
          MIRROR_VERTEX_X - MIRROR_FOCAL, CENTER_Y);
  fprintf(stderr, "# espelho parabolico: pico |u| = %.3f em t = %.3f (previsto %.3f)\n",
          parabolaPeak, parabolaTime, expectedTime);
  fprintf(stderr, "# parede reta:        pico |u| = %.3f\n", g_focusPeak);
  fprintf(stderr, "# ganho do foco = %.2f\n", parabolaPeak / g_focusPeak);
  return 0;
}
