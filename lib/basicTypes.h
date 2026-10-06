#ifndef M2_basicTypes_H
#define M2_basicTypes_H

#include "matrix.h"

typedef double (*RealFunction)(double x);
typedef double (*Real2Function)(double x, double t);
typedef void (*MatrixMuncher)(Matrix* m);

#endif
