/* Implementa uma representação de matriz densa orientada a linhas (veja: i_matrix_index)

   Algumas partes desse arquivo foram geradas pelo Claude Sonnet 5
   copiando o estilo do repositório github.com/padeir0/pao.
*/
#ifndef M2_MATRIX_H
#define M2_MATRIX_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define DEBUG 1

typedef struct {
  double* data;
  int rows;
  int columns;
} Matrix;

// calcula o offset do ponteiro `data`
static inline
int i_matrix_index(const Matrix* m, int row, int col) {
  return row * m->columns + col;
}

static inline
double matrix_at(const Matrix* m, int row, int col) {
  return m->data[i_matrix_index(m, row, col)];
}

static inline
void matrix_setAt(Matrix* m, int row, int col, double value) {
  m->data[i_matrix_index(m, row, col)] = value;
}

static inline
size_t matrix_length(const Matrix* m) {
  #if DEBUG
    assert(m->rows > 0);
    assert(m->columns > 0);
  #endif
  return (size_t)m->rows * (size_t)m->columns;
}

static inline
int matrix_sameShape(const Matrix* A, const Matrix* B) {
  return A->rows == B->rows && A->columns == B->columns;
}

/* Aloca uma matrix de ordem rows X columns
 * retorna NULL se a alocação falhar
*/
static inline
Matrix* matrix_new(int rows, int columns) {
  // UNSAFE: assumes `rows` and `columns` are positive; violating
  // this is a programming error and aborts the program.
  #if DEBUG
    assert(rows > 0);
    assert(columns > 0);
  #endif

  Matrix* m = (Matrix*)malloc(sizeof(*m));
  if (m == NULL) {
    return NULL;
  }

  size_t n = (size_t)rows * (size_t)columns;
  m->data = (double*)calloc(n, sizeof(double));
  if (m->data == NULL) {
    free(m);
    return NULL;
  }

  m->rows = rows;
  m->columns = columns;
  return m;
}

// returns false if failed to set
static inline
bool matrix_set(double* data, int rows, int columns, Matrix* m) {
  #if DEBUG
    assert(rows > 0);
    assert(columns > 0);
  #endif

  size_t n = (size_t)rows * (size_t)columns;

  if (matrix_length(m) != n) {
    free(m->data);
    m->data = (double*)calloc(n, sizeof(double));
    if (m->data == NULL) {
      free(m);
      return false;
    }
  }

  size_t i = 0;
  while (i < n) {
    m->data[i] = data[i];
    i++;
  }

  m->rows = rows;
  m->columns = columns;
  return m;
}

static inline
void matrix_setIdentity(Matrix* m) {
  #if DEBUG
    assert(m->rows == m->columns);
  #endif
  int i = 0;
  while (i < m->rows) {
    int j = 0;
    while (j < m->columns) {
      double value = (i == j) ? 1.0 : 0.0;
      m->data[i_matrix_index(m, i, j)] = value;
      j++;
    }
    i++;
  }
}

// libera a memória de uma matriz e seta o ponteiro como NULL
// pra evitar use-after-free
static inline
void matrix_free(Matrix** m) {
  if (m == NULL || *m == NULL) {
    return;
  }
  free((*m)->data);
  free(*m);
  *m = NULL;
}

static inline
void matrix_copy(const Matrix* source, Matrix* dest) {
  #if DEBUG
    assert(source != NULL); assert(dest != NULL);
    assert(matrix_sameShape(source, dest));
  #endif

  size_t length = matrix_length(source);
  size_t i = 0;
  while (i < length) {
    dest->data[i] = source->data[i];
    i++;
  }
}

static inline
Matrix* matrix_newCopy(const Matrix* source) {
  #if DEBUG
    assert(source != NULL);
  #endif
  Matrix* dest = matrix_new(source->rows, source->columns);
  matrix_copy(source, dest);
  return dest;
}

static inline
void matrix_swapRows(Matrix* A, int row1, int row2) {
  #if DEBUG
    assert(A != NULL);
    assert(row1 < A->rows);
    assert(row2 < A->rows);
  #endif
  double value1 = 0;
  double value2 = 0;
  int column = 0;
  while (column < A->columns) {
    value1 = matrix_at(A, row1, column);
    value2 = matrix_at(A, row2, column);
    matrix_setAt(A, row2, column, value1);
    matrix_setAt(A, row1, column, value2);
    column++;
  }
}

/* Implements the elementary operation 'A_i <- A_i + m * A_j'
   where 'A_i' means ith row of A and `m` is a scalar value.
*/
static inline
void matrix_rowMultAdd(Matrix* A, int row_i, int row_j, double multiple) {
  #if DEBUG
    assert(A != NULL);
    assert(row_i < A->rows);
    assert(row_j < A->rows);
    assert(multiple != NAN); // fuck NaNs!
  #endif

  int k = 0;
  while (k < A->columns) {
    double a_ik = matrix_at(A, row_i, k);
    double a_jk = matrix_at(A, row_j, k);

    double value = a_ik + multiple * a_jk;
    matrix_setAt(A, row_i, k, value);
    k++;
  }
}

// C = A + B; requer que A, B e C tenham as mesmas dimensões. Não aloca.
//
// A, B e C podem ser aliased livremente entre si, inclusive
// matrix_add(A, A, A), já que cada posição de C só depende
// da mesma posição de A e B.
static inline
void matrix_add(const Matrix* A, const Matrix* B, Matrix* C) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(C != NULL);
    assert(matrix_sameShape(A, B));
    assert(matrix_sameShape(A, C));
  #endif

  size_t n = (size_t)A->rows * (size_t)A->columns;
  size_t k = 0;
  while (k < n) {
    C->data[k] = A->data[k] + B->data[k];
    k++;
  }
}

// C = A - B; requer que A, B e C tenham as mesmas dimensões. Não aloca.
//
// Mesma garantia de aliasing que matrix_add: A, B e C podem
// ser o mesmo buffer em qualquer combinação.
static inline
void matrix_sub(const Matrix* A, const Matrix* B, Matrix* C) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(C != NULL);
    assert(matrix_sameShape(A, B));
    assert(matrix_sameShape(A, C));
  #endif

  size_t n = (size_t)A->rows * (size_t)A->columns;
  size_t k = 0;
  while (k < n) {
    C->data[k] = A->data[k] - B->data[k];
    k++;
  }
}

// C = scalar * A; requer que A e C tenham as mesmas dimensões. Não aloca.
//
// A e C podem ser o mesmo buffer (matrix_scalarMult(A, s, A) é válido).
static inline
void matrix_scalarMult(const Matrix* A, double scalar, Matrix* C) {
  #if DEBUG
    assert(A != NULL);
    assert(C != NULL);
    assert(matrix_sameShape(A, C));
  #endif

  size_t n = (size_t)A->rows * (size_t)A->columns;
  size_t k = 0;
  while (k < n) {
    C->data[k] = A->data[k] * scalar;
    k++;
  }
}

// C = A * B (produto matricial padrão); requer A->columns == B->rows,
// C->rows == A->rows e C->columns == B->columns. Não aloca.
//
// DIFERENTE das operações acima, o produto matricial não pode ser
// escrito no lugar: cada C[i][j] acumula vários A[i][k]*B[k][j], e
// sobrescrever C cedo demais destruiria valores de A ou B que ainda
// seriam lidos em iterações seguintes. Por isso esse aliasing não é
// suportado -- C não pode compartilhar buffer com A nem com B. Chamar
// a função nessas condições é erro de programação e aborta o
// programa, do mesmo jeito que as outras UNSAFE-preconditions acima.
static inline
void matrix_mult(const Matrix* A, const Matrix* B, Matrix* C) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(C != NULL);
    assert(A->columns == B->rows);
    assert(C->rows == A->rows);
    assert(C->columns == B->columns);
    // UNSAFE: C aliasing A or B is not supported, see comment above.
    assert(A->data != C->data);
    assert(B->data != C->data);
  #endif

  size_t n = (size_t)C->rows * (size_t)C->columns;
  size_t z = 0;
  while (z < n) {
    C->data[z] = 0.0;
    z++;
  }

  int i = 0;
  while (i < A->rows) {
    int k = 0;
    while (k < A->columns) {
      double aik = A->data[i_matrix_index(A, i, k)]; // NOTE(1)
      int j = 0;
      while (j < B->columns) {
        int idx = i_matrix_index(C, i, j);
        C->data[idx] += aik * B->data[i_matrix_index(B, k, j)];
        j++;
      }
      k++;
    }
    i++;
  }
  /* NOTE(1): loops are ordered row-k-column rather than the naive
              row-column-k triple loop, so the innermost loop walks
              `B` and `C` with unit stride, which is far more
              cache-friendly. */
}

// C = A^T; requer C->rows == A->columns e C->columns == A->rows. Não aloca.
//
// Se A e C compartilham o buffer (A->data == C->data), a matriz
// precisa ser quadrada: só nesse caso dá pra transpor no lugar,
// trocando cada par (i,j)/(j,i). Fora do caso quadrado-aliased,
// assume-se que os buffers de A e C não se sobrepõem (UNSAFE).
static inline
void matrix_transpose(const Matrix* A, Matrix* C) {
  #if DEBUG
    assert(A != NULL);
    assert(C != NULL);
    assert(C->rows == A->columns);
    assert(C->columns == A->rows);
  #endif

  if (A->data == C->data) {
    // UNSAFE: in-place transpose requires a square matrix.
    assert(A->rows == A->columns);
    int i = 0;
    while (i < C->rows) {
      int j = i + 1;
      while (j < C->columns) {
        int idxUpper = i_matrix_index(C, i, j);
        int idxLower = i_matrix_index(C, j, i);
        double tmp = C->data[idxUpper];
        C->data[idxUpper] = C->data[idxLower];
        C->data[idxLower] = tmp;
        j++;
      }
      i++;
    }
    return;
  }

  int i = 0;
  while (i < A->rows) {
    int j = 0;
    while (j < A->columns) {
      C->data[i_matrix_index(C, j, i)] =
          A->data[i_matrix_index(A, i, j)];
      j++;
    }
    i++;
  }
}

static inline
bool matrix_equals(const Matrix* A, const Matrix* B, double error) {
  #if DEBUG
    assert(A != NULL);
    assert(B != NULL);
    assert(error >= 0);
  #endif

  if (matrix_sameShape(A, B) == false) {
    return false;
  }

  int rows = A->rows;
  int columns = A->columns;
  int i;
  int j;
  
  i = 0;
  while (i < rows) {
    j = 0;
    while (j < columns) {
      if (fabs(matrix_at(A, i, j) - matrix_at(B, i, j)) > error) {
        return false;
      }
      j++;
    }
    i++;
  }
  return true;
}

/* implementa a norma de frobenius ao quadrado */
double matrix_normSquared(const Matrix* X) {
  #if DEBUG
    assert(X != NULL);
  #endif

  double out = 0;

  int i = 0;
  int j = 0;
  while (i < X->rows) {
    j = 0;
    while (j < X->columns) {
      double value = matrix_at(X, i, j);
      out += value * value;
      j++;
    }
    i++;
  }

  return out;
}


/* BEGIN: SNPRINT */

// Retorna a maior largura, em caracteres, dentre as células de `A`
// formatadas com "%.*f" e `precision` casas decimais.
static inline
int i_matrix_printingColWidth(const Matrix* A, int precision) {
  int width = 0;
  int i = 0;
  while (i < A->rows) {
    int j = 0;
    while (j < A->columns) {
      int w = snprintf(NULL, 0, "%.*f", precision, A->data[i_matrix_index(A, i, j)]);
      if (w > width) {
        width = w;
      }
      j++;
    }
    i++;
  }
  return width;
}

// Calcula quantos bytes `matrix_snprint` precisa pra
// escrever `A` com `precision` casas decimais por célula.
static inline
size_t matrix_printingSize(const Matrix* A, int precision) {
  #if DEBUG
    assert(A != NULL);
    assert(precision >= 0);
  #endif

  size_t colWidth = (size_t)i_matrix_printingColWidth(A, precision);
  // cada linha: "[ " + colunas (colWidth cada) + separadores
  // (2 espaços entre colunas) + " ]"
  size_t rowBytes = 2 + (size_t)A->columns * colWidth
                       + (size_t)(A->columns - 1) * 2 + 2;
  // linhas separadas por '\n', sem '\n' depois da última
  return (size_t)A->rows * rowBytes + (size_t)(A->rows - 1);
}

/* Escreve uma representação legível de `A` em `buffer`, com colunas
 * alinhadas à direita e `precision` casas decimais por célula. Só
 * escreve se `size` comportar o resultado inteiro; do contrário não
 * escreve nada e retorna 0 (mesmo contrato de `natural_snprint`).
 * Não inclui `\0` no fim, nem `\n` depois da última linha.
*/
static inline
size_t matrix_snprint(const Matrix* A, int precision, char* buffer, size_t size) {
  #if DEBUG
    assert(A != NULL);
    assert(buffer != NULL);
    assert(precision >= 0);
  #endif

  size_t needed = matrix_printingSize(A, precision);
  if (needed > size) {
    return 0;
  }

  int colWidth = i_matrix_printingColWidth(A, precision);
  char cell[512]; // UNSAFE: suficiente pra qualquer double em ponto
                   // fixo (DBL_MAX tem ~309 dígitos); ver assert abaixo.
  assert(colWidth < (int)sizeof(cell));

  char* b = buffer;
  int i = 0;
  while (i < A->rows) {
    *b++ = '[';
    *b++ = ' ';
    int j = 0;
    while (j < A->columns) {
      double v = A->data[i_matrix_index(A, i, j)];
      snprintf(cell, sizeof(cell), "%*.*f", colWidth, precision, v);
      memcpy(b, cell, (size_t)colWidth);
      b += colWidth;
      if (j + 1 < A->columns) {
        *b++ = ' ';
        *b++ = ' ';
      }
      j++;
    }
    *b++ = ' ';
    *b++ = ']';
    if (i + 1 < A->rows) {
      *b++ = '\n';
    }
    i++;
  }

  return needed;
}

// imprime `A` no stdout usando matrix_prettyPrint.
static inline
void matrix_print(const Matrix* A, int precision) {
  size_t needed = matrix_printingSize(A, precision);
  char* buffer = (char*)malloc(needed);
  if (buffer == NULL) {
    fprintf(stderr, "out of memory while printing matrix\n");
    exit(1);
  }
  size_t written = matrix_snprint(A, precision, buffer, needed);
  printf("%.*s\n", (int)written, buffer);
  free(buffer);
}
/* END: PRETTY PRINT */

#endif
