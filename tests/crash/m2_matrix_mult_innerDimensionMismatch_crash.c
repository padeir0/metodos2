#include "../../lib/matrix.h"

int main(void) {
  Matrix* a = matrix_new(2, 3);
  Matrix* b = matrix_new(4, 2); /* deveria ter 3 linhas, não 4 */
  Matrix* c = matrix_new(2, 2);
  if (a == NULL || b == NULL || c == NULL) {
    return 1;
  }

  /* matrix_mult requires A->columns == B->rows */
  matrix_mult(a, b, c);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&c);
  return 0;
}
