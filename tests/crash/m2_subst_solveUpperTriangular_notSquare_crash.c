#include "../../lib/matrix.h"
#include "../../lib/01_subst.h"

int main(void) {
  Matrix* a = matrix_new(2, 3); /* não-quadrada: rows != columns */
  Matrix* x = matrix_new(3, 1);
  Matrix* b = matrix_new(3, 1); /* B->rows == A->columns, então só a
                                    asserção de quadratura falha */
  if (a == NULL || x == NULL || b == NULL) {
    return 1;
  }

  /* matrix_solveUpperTriangular requires A->rows == A->columns */
  matrix_solveUpperTriangular(a, x, b);

  matrix_free(&a);
  matrix_free(&x);
  matrix_free(&b);
  return 0;
}
