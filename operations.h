#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "basic_funcs.h"
#include <cstdio>

double scalar_product(int n, double* x, double* y, int p, int k);
void mult_sub_vector(int n, double* x, double* y, double t, int p, int k);

void matrix_mult_vector_msr(int n, double* A, int* I,
  double* x, double* r, int p, int k);



#endif
