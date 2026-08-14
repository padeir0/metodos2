#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 2);
  if (a == NULL) {
    return 1;
  }

  /* matrix_rowMultAdd requires both row indices to be < A->rows */
  matrix_rowMultAdd(a, 2, 0, 1.0);

  matrix_free(&a);
  return 0;
}
