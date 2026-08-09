#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 3); /* não-quadrada */
  if (a == NULL) {
    return 1;
  }

  /* transpor no próprio lugar (C aliasa A) só é válido pra matriz
     quadrada: A e A^T têm formas diferentes quando A não é
     quadrada, então não cabem no mesmo buffer. */
  matrix_transpose(a, a);

  matrix_free(&a);
  return 0;
}
