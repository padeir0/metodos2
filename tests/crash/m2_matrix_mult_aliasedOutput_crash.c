#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* b = matrix_new(2, 2);
  if (a == NULL || b == NULL) {
    return 1;
  }
  matrix_setIdentity(a);
  matrix_setIdentity(b);

  /* matrix_mult forbids C aliasing A or B: como C acumula os
     produtos parciais, ler A ou B do mesmo buffer que já está
     sendo escrito dá resultado errado. Aqui C aliasa A. */
  matrix_mult(a, b, a);

  matrix_free(&a);
  matrix_free(&b);
  return 0;
}
