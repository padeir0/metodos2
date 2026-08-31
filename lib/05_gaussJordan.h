#include "matrix.h"
#include "01_subst.h"

#include <math.h>
#include <float.h>

#ifndef M2_GAUSSJORDAN_H
#define M2_GAUSSJORDAN_H

static inline
int i_matrix_findLargestRow(const Matrix* A, int row_start, int column, double error) {
  // usamos apenas valores em módulo, o minimo é zero
  double largest = 0;
  int selected = row_start;
  int row = row_start;
  while (row < A->rows) {
    double value = fabs(matrix_at(A, row, column));
    if (largest < value) {
      largest = value;
      selected = row;
    }
    row++;
  }
  if (fabs(largest-0) < error) {
    return -1;
  }
  return selected;
}

/* Here "sq" means "square" because this expects a square matrix.
   This function modifies both of it's pointer operands.
*/
static inline
bool matrix_gaussJordan(Matrix* A, Matrix* B, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(A->columns == A->rows);
    assert(B->rows == A->columns);
    assert(B->columns == 1);
  #endif
  int i;

 // cria a matriz extendida
  Matrix* AB = matrix_append(A, B);

  int k = 0;
  while (k < A->rows) {
    int bestRow = i_matrix_findLargestRow(AB, k, k, error);

    if (bestRow == -1) {
      // the matrix is not invertible
      matrix_free(&AB);
      return false;
    }

    matrix_swapRows(AB, k, bestRow);

    double a_kk = matrix_at(AB, k, k);
    matrix_rowMult(AB, k, 1.0/a_kk);

    i = k+1;
    while (i < A->rows) {
      double multiple = -matrix_at(AB, i, k);
      matrix_rowMultAdd(AB, i, k, multiple);
      i++;
    }
    i = k-1;
    while (i >= 0) {
      double multiple = -matrix_at(AB, i, k);
      matrix_rowMultAdd(AB, i, k, multiple);
      i--;
    }
    k++;
  }

  /* copia as partes de AB de volta pra A e B usando
     a função copyBlock (que copia um bloco da matriz input
     pra um bloco da matriz output).
  */
  matrix_copyBlock(AB, 0, A->rows, 0, A->columns,
                    A, 0, A->rows, 0, A->columns);
  matrix_copyBlock(AB, 0, B->rows, A->columns, A->columns+B->columns,
                    B, 0, B->rows, 0, B->columns);
  matrix_free(&AB);
  return true;
}
#endif
