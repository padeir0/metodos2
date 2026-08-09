#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* c = matrix_new(2, 3); /* deveria ser 3x2 */
  if (a == NULL || c == NULL) {
    return 1;
  }

  /* matrix_transpose requires C->rows == A->columns
     && C->columns == A->rows */
  matrix_transpose(a, c);

  matrix_free(&a);
  matrix_free(&c);
  return 0;
}
