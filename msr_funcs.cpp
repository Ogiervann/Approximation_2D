#include "msr_funcs.h"

int min_residual_msr_matrix(int n, double* A, int* I, double* b, double* x,
  double*r, double*u, double*v, double eps, int max_it, int p, int k){
    double prec, b_norm2, tau, c1, c2;
    int it;
    b_norm2 = scalar_product(n, b, b, p, k);
    prec = b_norm2*eps*eps;
    matrix_mult_vector_msr(n, A, I, x, r, p, k); //r = Ax
    mult_sub_vector(n, r, b, 1, p, k); // r -= 1*b
    for(it = 0; it < max_it; it++){
/*
      if(k == 0){
        printf("max_it: %d\n", max_it);
        printf("b_norm2: %lf\n", b_norm2);
        printf("prec: %e\n", prec);
        for(int i = 0; i < n; i++){
          printf("%lf ", x[i]);
        }
        printf("x\n");
      }
      reduce_sum(p);
        if(k == 0){
          for(int i = 0; i < n; i++){
            printf("%lf ", r[i]);
          }
          printf("r\n");
        }
        reduce_sum(p);
        if(k == 0){
            for(int i = 0; i < n; i++){
              printf("%lf ", b[i]);
            }
            printf("b\n");
          }
          reduce_sum(p);
          if(k == 0){
              for(int i = 0; i < n; i++){
                printf("%e ", A[i]);
              }
              printf("A\n");
            }
            reduce_sum(p);

*/

      apply_preconditioner_msr_matrix(n, A, I, r, v, p, k);// Mv = r
      matrix_mult_vector_msr(n, A, I, v, u, p, k); // u = Av
/*
      if(k == 0){
          for(int i = 0; i < n; i++){
            printf("%lf ", v[i]);
          }
          printf("v\n");
        }
        reduce_sum(p);
        if(k == 0){
            for(int i = 0; i < n; i++){
              printf("%lf ", u[i]);
            }
            printf("u\n");
          }
          reduce_sum(p);
*/
      c1 = scalar_product(n, u, r, p, k);
      c2 = scalar_product(n, u, u, p, k);
      if(c1 < prec || c2 < prec) break;
      tau = c1/c2;
      mult_sub_vector(n, x, v, tau, p, k); // x -= tau*v
      mult_sub_vector(n, r, u, tau, p, k); // r -= tau*u
    }
    if(it > max_it) return -1;// в x последнее приближение
    return it;
}

int min_residual_msr_matrix_full(int n, double* A, int* I, double* b,
  double* x/* начальное приближение */, double* r, double* u, double* v,
  double eps, int maxit, int maxsteps, int p, int k){
    int step, ret, its = 0;
    for(step = 0; step < maxsteps; step++){
      ret = min_residual_msr_matrix(n, A, I, b, x, r, u, v, eps, maxit, p, k);
      if(ret >= 0){its += ret; break;}
      its+=maxit;//в x новое начальное приближение
    }
    if(step >= maxsteps)return -1;
    return its;
}


void apply_preconditioner_msr_matrix(int n, double* A, int* I, double* r,
  double* v, int p, int k){
    //A = L+D+L^T, M = (D+L)D^-1(D+L)^T, Mv=r
    int i, j, i1, i2, j1, j2;
    double s;
    double param = 1;
//Начало теоретически верного
    thread_rows(n, p, k, i1, i2);
    memcpy(v+i1, r+i1, (i2-i1)*sizeof(double));
    reduce_sum(p);
    for (i = i1; i < i2; i++) {
      v[i]/=A[i];
      s = 0;
      j1 = I[i];
      j2 = I[i+1];
      for (j = j1; j < j2; j++)
        if (I[j] >= i1 && I[j] < i)
          s += A[j] * v[I[j]] * param;
      v[i] -=  s/ A[i];
    }
    for(i = i1; i < i2; i++)v[i] = v[i] * A[i];


    for (i = i2 - 1; i >= i1; i--) {
      v[i]/=A[i];

      s = 0;
      j1 = I[i];
      j2 = I[i+1];
      for (j = j1; j < j2; j++)
        if (I[j] <= i2 - 1 && I[j] > i)
          s += A[j] * v[I[j]] * param;
      v[i] -= s/A[i];
    }

    for (i = i2 - 1; i >= i1; i--)
      v[i] *= (param * (2 - param));

    reduce_sum(p);

}
