#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 2);
  if (a == NULL) {
    return 1;
  }

  /* matrix_swapRows requires both row indices to be < A->rows */
  matrix_swapRows(a, 0, 2);

  matrix_free(&a);
  return 0;
}
