#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* c = matrix_new(3, 3);
  if (a == NULL || c == NULL) {
    return 1;
  }

  /* matrix_scalarMult requires A and C to share the same shape */
  matrix_scalarMult(a, 2.0, c);

  matrix_free(&a);
  matrix_free(&c);
  return 0;
}
