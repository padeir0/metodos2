#include "../../lib/matrix.h"
#include "../../lib/01_subst.h"

int main(void) {
  Matrix* a = matrix_new(2, 2);
  if (a == NULL) {
    return 1;
  }
  matrix_setIdentity(a);

  /* matrix_isUpperTriangular requires error >= 0 */
  (void)matrix_isUpperTriangular(a, -1.0);

  matrix_free(&a);
  return 0;
}
