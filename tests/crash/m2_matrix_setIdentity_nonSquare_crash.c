#include "../../lib/matrix.h"

int main(void) {
  Matrix* m = matrix_new(2, 3);
  if (m == NULL) {
    return 1;
  }

  /* matrix_setIdentity requires a square matrix */
  matrix_setIdentity(m);

  matrix_free(&m);
  return 0;
}
