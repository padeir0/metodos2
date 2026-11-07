#include "matrix.h"
#include "basicTypes.h"

#include <float.h>

#ifndef M2_wave_H
#define M2_wave_H

/* Preenche os zeros da fronteira e dos obstáculos. */
static inline
void i_de_waveSetZeroes(Matrix* grid, Real2Function g, double h) {
  int dim = grid->rows;
  int i = 0;
  while (i < dim) {
    int j = 0;
    while (j < dim) {
      double x = i*h;
      double y = j*h;

      if (g(x,y) > 0) {
        matrix_setAt(grid, i, j, 0);
      }

      if (i == 0 || j == 0 || i == dim-1 || j == dim-1) {
        matrix_setAt(grid, i, j, 0);
      }
      j++;
    }
    i++;
  }
}

/*
Soluciona a EDP da onda 2D:
    u_{tt}(x, y, t) = c^2 * (u_{xx}(x, y, t) + u_{yy}(x, y, t))
    u_{t}(x, y, 0) = 0
    u(x,y,0) = f(x,y)
    u(x, y, t) = g(x, y)
    u(0, y, t) = 0
    u(1, y, t) = 0
    u(x, 0, t) = 0
    u(x, 1, t) = 0

Onde f:R^2 -> R é uma função que define as condições iniciais,
g:R^2 -> R é uma função que define um obstáculo e as fronteiras
do quadrado [0,1]x[0,1] definem uma barreira.
Isso é uma condição de Dirichlet onde as ondas são refletidas perfeitamente.
*/
static inline
void de_solveWave(Real2Function f, Real2Function g, double C, MatrixMuncher munch, double h, double d, double time) {
  int dim = (int)lround(1 / h) + 1;
  Matrix* grid_prev = matrix_new(dim, dim); // t-1
  Matrix* grid_curr = matrix_new(dim, dim); // t
  Matrix* grid_next = matrix_new(dim, dim); // t+1

  matrix_setAll(grid_prev, 0);

  // condição inicial (seta grid_curr)
  {
    int i = 0;
    while (i < dim) {
      int j = 0;
      while (j < dim) {
        double u_ij = f(i*h, j*h);
        matrix_setAt(grid_curr, i, j, u_ij);
        j++;
      }
      i++;
    }
    // fronteiras
    i_de_waveSetZeroes(grid_curr, g, h);
    munch(grid_curr);
  }

  int k = 1;
  while (k*d < time) {
    int i = 1;
    while (i < dim-1) {
      int j = 1;
      while (j < dim-1) {
        double u_prev   = matrix_at(grid_prev, i, j);
        double u_curr   = matrix_at(grid_curr, i, j);
        double u_left   = matrix_at(grid_curr, i-1, j);
        double u_right  = matrix_at(grid_curr, i+1, j);
        double u_top    = matrix_at(grid_curr, i, j-1);
        double u_bottom = matrix_at(grid_curr, i, j+1);

        double delta = (d * C) / h;
        double laplacian = (delta*delta) * (u_bottom + u_top + u_right + u_left - 4 * u_curr);

        double u_next = 2 * u_curr - u_prev + laplacian;
        if (k == 1) {
          u_next = u_curr + laplacian/2;
        } else {
          u_next = 2 * u_curr - u_prev + laplacian;
        }

        matrix_setAt(grid_next, i, j, u_next);
        j++;
      }
      i++;
    }
    { // swap
      Matrix* temp = grid_prev;
      grid_prev = grid_curr;
      grid_curr = grid_next;
      grid_next = temp;
    }

    i_de_waveSetZeroes(grid_curr, g, h);
    munch(grid_curr);

    k++;
  }

  matrix_free(&grid_prev);
  matrix_free(&grid_curr);
  matrix_free(&grid_next);
}

#endif
