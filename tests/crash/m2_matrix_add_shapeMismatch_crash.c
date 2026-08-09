#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 2);
  Matrix* b = matrix_new(3, 3);
  Matrix* c = matrix_new(2, 2);
  if (a == NULL || b == NULL || c == NULL) {
    return 1;
  }

  /* matrix_add requires A, B and C to share the same shape */
  matrix_add(a, b, c);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  return 0;
}
