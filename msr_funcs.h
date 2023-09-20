#ifndef MSR_FUNCS_H
#define MSR_FUNCS_H

#include <cstring>
#include <cstdio>
#include "operations.h"


int min_residual_msr_matrix(int n, double* A, int* I, double* b, double* x,
  double*r, double*u, double*v, double eps, int maxit, int p, int k);

int min_residual_msr_matrix_full(int n, double* A, int* I, double* b,
  double* x/* начальное приближение */, double* r, double* u, double* v,
  double eps, int maxit, int maxsteps, int p, int k);

void apply_preconditioner_msr_matrix(int n, double* A, int* I, double* r,
  double* v, int p, int k);



#endif
