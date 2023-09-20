#include "operations.h"



double scalar_product(int n, double* x, double* y, int p, int k){
  int i, i1, i2; double s = 0;
  thread_rows(n, p, k, i1, i2);
  for(i = i1; i < i2; i++)s+=x[i]*y[i];
  reduce_sum(p, &s, 1);//reduce_sum_det!!
  return s;
}

void mult_sub_vector(int n, double* x, double* y, double t, int p, int k){
  //x -= t*y
  int i, i1, i2;
  thread_rows(n, p, k, i1, i2);
  for(i = i1; i < i2; i++)x[i] -= t*y[i];
}

void matrix_mult_vector_msr(int n, double* A, int* I,
  double* x, double* r, int p, int k){
  //r = Ax
  int i, i1, i2, j = 0, j0, j1, j2;
  double s;
  thread_rows(n, p, k, i1, i2);
  for(i = i1; i < i2; i++){
    j1 = I[i];
    j2 = I[i+1];
    s=A[i]*x[i];
    for(j0 = j1; j0 < j2; j0++){
      j = I[j0];
      s += A[j0]*x[j];
    }
    r[i] = s;
  }
  reduce_sum(p);
}
