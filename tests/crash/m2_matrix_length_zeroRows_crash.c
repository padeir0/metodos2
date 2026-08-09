#include "../../lib/matrix.h"

int main(void) {
  /* matrix_length requires rows > 0. Construímos a struct na mão,
     em vez de usar matrix_new(), já que matrix_new() tem essa
     mesma precondição e abortaria antes da gente chegar em
     matrix_length(). */
  Matrix m = { .data = NULL, .rows = 0, .columns = 3 };

  (void)matrix_length(&m);

  return 0;
}
