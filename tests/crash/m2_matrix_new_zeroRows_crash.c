#include "../../lib/matrix.h"

int main(void) {
  /* matrix_new requires rows > 0 */
  Matrix* m = matrix_new(0, 3);

  matrix_free(&m);
  return 0;
}
