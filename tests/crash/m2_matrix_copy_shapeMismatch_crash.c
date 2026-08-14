#include "../../lib/matrix.h"

int main(void) {
  Matrix* source = matrix_new(2, 2);
  Matrix* dest = matrix_new(3, 3);
  if (source == NULL || dest == NULL) {
    return 1;
  }

  /* matrix_copy requires source and dest to share the same shape */
  matrix_copy(source, dest);

  matrix_free(&source);
  matrix_free(&dest);
  return 0;
}
