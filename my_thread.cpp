#include "my_thread.h"
#include <cmath>

void* thread_func(void* ptr){
  Args* args = (Args*) ptr;
  Results *res = args->res;
  double a = args->a;
  double b = args->b;
  double c = args->c;
  double d = args->d;

  int nx = args->nx;
  int ny = args->ny;

  double eps = args->eps;

  int maxit = args->maxit;

  double (*f)(double, double) = args->f;

  double err_f = args->err;
  int err_p = args->err_p;

  int k = args->k;
  int p = args->p;

  double* A = args->A;
  int* I = args->I;
  double* B = args->B;
  double* x = args->x;
  double* r = args->r;
  double* u = args->u;
  double* v = args->v;

  //int status = 0;
  double r1= -1, r2 = -1, r3 = -1, r4 = -1;

  cpu_set_t cpu;
  CPU_ZERO(&cpu);
  int n_cpus = get_nprocs();
  int cpu_id = n_cpus - 1 - (k % n_cpus);
  CPU_SET(cpu_id, &cpu);
  pthread_t tid = pthread_self();
  pthread_setaffinity_np(tid, sizeof(cpu), &cpu);

  int n = (nx+1)*(ny+1);
  int i1, i2;
  thread_rows(n, p, k, i1, i2);
  memset(x + i1, 0, (i2-i1) * sizeof(double));
  reduce_sum(p);
double hx = (b-a)/(nx);
  double hy = (d-c)/(ny);

if(k == 0){
  fill_I(nx, ny, I);
}

reduce_sum(p);

  int res_err = fill_IA(nx, ny, hx, hy, I, A, p, k);
  if(res_err < 0){

    if(k==0)res->status = -1;
    return 0;
  }
  fill_b(nx, ny, hx, hy, a, c, f, B, p, k, err_p, err_f);
  double time = get_full_time();
  int it = min_residual_msr_matrix(n, A, I, B, x, r, u, v, eps, maxit, p, k);
  time = get_full_time() - time;
  res[k].alg_time = time;
/*
  if(k == 0){
    for(int i = 0; i < n; i++){
      printf("%lf ", x[i]);
    }
    printf("\n");
  }
  reduce_sum(p);

  if(k == 0){
    double x11, y11;
    while(true){
      if(!scanf("%lf\n", &x11))break;
      if(!scanf("%lf\n", &y11))break;
      int br = x11+y11;
      if(br == 0){
        break;
      }
      printf("%lf %lf", f(x11, y11), Pf(x, x11, y11, nx, ny, a, c, hx, hy));
    }
  }
  reduce_sum(p);
*/
if(it > 0){
  time = get_full_time();
  r1 = find_r1(x, f, nx, ny, hx, hy, a, c, p, k);
  r2 = find_r2(x, f, nx, ny, hx, hy, a, c, p, k);
  r3 = find_r3(x, f, nx, ny, hx, hy, a, c, p, k);
  r4 = find_r4(x, f, nx, ny, hx, hy, a, c, p, k);

  res[k].it = it;
  res[k].r1 = r1;
  res[k].r3 = r3;
reduce_sum(p);
  r1 = res[0].r1;
  r3 = res[0].r3;
  for(int i = 1; i < p; i++){
    if(res[i].r1 > r1)
      r1 = res[i].r1;
    if(res[i].r1 > r1)
      r1 = res[i].r1;
  }
  reduce_sum(p, &r2, 1);
  res[k].r2 = r2;
  reduce_sum(p, &r4, 1);
  res[k].r4 = r4;
  res[k].r1 = r1;
  res[k].r3 = r3;
  time = get_full_time() - time;
  res[k].res_time = time;
}


reduce_sum(p);
*(args->in_progress_flag) = false;
  args->gresults.in_progress_flag = false;

  return 0;
}
