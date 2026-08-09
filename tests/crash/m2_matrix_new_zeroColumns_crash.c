#include "../../lib/matrix.h"

int main(void) {
  /* matrix_new requires columns > 0 */
  Matrix* m = matrix_new(3, 0);

  matrix_free(&m);
  return 0;
}
