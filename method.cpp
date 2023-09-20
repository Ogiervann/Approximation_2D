#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "my_thread.h"




int main(int argc, char* argv[]){
  int task = 3;
  double a, b, c, d;
  int nx, ny;
  int k_func;
  double eps;
  int maxit;
  int p;
  double r1 = -1, r2 = -1, r3 = -1, r4 = -1;
  double t1 = -1, t2 = -1;
  int it = -1;
  //unsigned int start, end;

  double *A;
  int* I;
  double *B;
  double *x;
  double *r;
  double* u;
  double* v;
  Args *args; Results *res;


  if(!(argc == 11 && sscanf(argv[1], "%lf", &a) == 1 &&
    sscanf(argv[2], "%lf", &b) == 1 && sscanf(argv[3], "%lf", &c) == 1 &&
    sscanf(argv[4], "%lf", &d) == 1 && sscanf(argv[5], "%d", &nx) == 1 &&
    sscanf(argv[6], "%d", &ny) == 1 && sscanf(argv[7], "%d", &k_func) == 1 &&
    sscanf(argv[8], "%lf", &eps) == 1 && sscanf(argv[9], "%d", &maxit) == 1 &&
    sscanf(argv[10], "%d", &p) == 1)){
      printf("Usage %s a b c d nx ny k eps maxit p\n", argv[0]);
      return 1;
  }



  int lm = get_len_msr(nx, ny);
  int n = (nx+1)*(ny+1);
  A = new double[n+1+lm];
  I = new int[n+1+lm];

  B = new double[n];
  x = new double[n];
  r = new double[n];
  u = new double[n];
  v = new double[n];


  args = new Args[p]; res = new Results[p];
  if(A == NULL || I == NULL || B == NULL || x == NULL || r == NULL || u == NULL || v == NULL){
    printf("Not enough memory\n");
    if(A != NULL) delete[] A;
    if(I != NULL) delete[] I;
    if(B != NULL) delete[] B;
    if(x != NULL) delete[] x;
    if(r != NULL) delete[] r;
    if(u != NULL) delete[] u;
    if(v != NULL) delete[] v;
    return 1;
  }

  if(fill_I(nx, ny, I) != n + 1 + lm){
    printf("Error while filling I\n");
    if(A != NULL) delete[] A;
    if(I != NULL) delete[] I;
    if(B != NULL) delete[] B;
    if(x != NULL) delete[] x;
    if(r != NULL) delete[] r;
    if(u != NULL) delete[] u;
    if(v != NULL) delete[] v;
    return 1;

  }



  for(int k = 0; k < p; k++){
    args[k].res = res;
    args[k].a = a;
    args[k].b = b;
    args[k].c = c;
    args[k].d = d;
    args[k].nx = nx;
    args[k].ny = ny;
    args[k].eps = eps;
    args[k].maxit = maxit;
    args[k].k_func = k_func;
    args[k].set_func();
    args[k].k = k;
    args[k].p = p;

    args[k].A = A;
    args[k].I = I;
    args[k].B = B;
    args[k].x = x;
    args[k].r = r;
    args[k].u = u;
    args[k].v = v;
  }


  //start = get_full_time();
  for(int k = 1; k < p; k++){
    if(pthread_create(&args[k].tid, 0, thread_func, args+k)){
      printf("Cannot create thread %d\n", k);
      std::abort();
    }//if
  }//for
  thread_func(args+0);
  for (int k = 1; k < p; k++) {
    pthread_join(args[k].tid, 0);
  }
  //end = get_full_time();
  //double global_time = end-start;

  r1 = res[0].r1;
  r2 = res[0].r2;
  r3 = res[0].r3;
  r4 = res[0].r4;
  it = res[0].it;
  printf (
  "%s : Task = %d R1 = %e R2 = %e R3 = %e R4 = %e T1 = %.2f T2 = %.2f\
It = %d E = %e K = %d Nx = %d Ny = %d P = %d\n",
  argv[0], task, r1, r2, r3, r4, t1, t2, it, eps, k_func, nx, ny, p);


  if(A != NULL) delete[] A;
  if(I != NULL) delete[] I;
  if(B != NULL) delete[] B;
  if(x != NULL) delete[] x;
  if(r != NULL) delete[] r;
  if(u != NULL) delete[] u;
  if(v != NULL) delete[] v;
  delete[] res;
  delete[] args;
  return 0;
}
