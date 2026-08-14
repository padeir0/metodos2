#include "../../lib/matrix.h"
#include "../../lib/01_subst.h"
#include "../../lib/02_gauss.h"

int main(void) {
  Matrix* a = matrix_new(2, 3); /* não-quadrada */
  Matrix* b = matrix_new(2, 1);
  if (a == NULL || b == NULL) {
    return 1;
  }

  /* matrix_sqGaussianElimination requires A->columns == A->rows */
  (void)matrix_sqGaussianElimination(a, b, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return 0;
}
