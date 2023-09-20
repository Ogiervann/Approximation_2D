#include "basic_funcs.h"


void ij2l(int nx, int /*ny*/, int i, int j, int &l){
  l = i+j*(nx+1);
}

void l2ij(int nx, int /*ny*/, int &i, int &j, int l){
  j = l/(nx+1);
  i = l-j*(nx+1);
}

int get_len_msr(int nx, int ny){
  //общая длина (nx-1)*(ny-1)+1+get_len_msr(nx,ny);
  return (nx-1)*(ny-1)*6+(2*(nx-1)+2*(ny-1))*4+2*3+2*2;
}


void thread_rows(int n, int p, int k, int&i1, int &i2){
  i1 = n*k; i1 /= p; i2 = n*(k+1); i2 /= p;
}

void reduce_sum(int p, double* a, int n){
  static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
  static int t_in = 0;
  static int t_out = 0;
  static double*r = nullptr;
  int i;
  if(p <= 1)return;
  pthread_mutex_lock(&m);
  if(r == nullptr){
    r = a;
  }
  else{
    for(i = 0; i < n; i++) r[i]+=a[i];
  }
  t_in++;
  if(t_in >= p){
    t_out = 0;
    pthread_cond_broadcast(&c_in);
  }else{
    while(t_in<p){
      pthread_cond_wait(&c_in, &m);
    }
  }


  if(r != a){
    for(i = 0; i < n; i++) a[i] = r[i];
  }
  t_out++;
  if(t_out >=p){
    t_in = 0;
    r = nullptr;
    pthread_cond_broadcast(&c_out);
  }else{
    while(t_out < p){
      pthread_cond_wait(&c_out, &m);
    }
  }
  pthread_mutex_unlock(&m);
}


void reduce_sum_int(int p, int* a, int n){
  static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
  static int t_in = 0;
  static int t_out = 0;
  static int* r = nullptr;
  int i;
  if(p <= 1)return;
  pthread_mutex_lock(&m);
  if(r == nullptr){
    r = a;
  }
  else{
    for(i = 0; i < n; i++) r[i]+=a[i];
  }
  t_in++;
  if(t_in >= p){
    t_out = 0;
    pthread_cond_broadcast(&c_in);
  }else{
    while(t_in<p){
      pthread_cond_wait(&c_in, &m);
    }
  }


  if(r != a){
    for(i = 0; i < n; i++) a[i] = r[i];
  }
  t_out++;
  if(t_out >=p){
    t_in = 0;
    r = nullptr;
    pthread_cond_broadcast(&c_out);
  }else{
    while(t_out < p){
      pthread_cond_wait(&c_out, &m);
    }
  }
  pthread_mutex_unlock(&m);
}

double get_full_time(){
  struct timeval buf;
  gettimeofday(&buf, NULL);
  return buf.tv_sec + buf.tv_usec / 1.e6;
}

double get_CPU_time(){
  struct rusage buf;
  getrusage(RUSAGE_THREAD, &buf);
  return buf.ru_utime.tv_sec + buf.ru_utime.tv_usec / 1.e6;
}


double f(double x, double y, int k){
  if(k == 0)return 1;
  if(k == 1)return x;
  if(k == 2)return y;
  if(k == 3)return x+y;
  if(k == 4)return sqrt(x*x+y*y);
  if(k == 5)return x*x+y*y;
  if(k == 6)return exp(x*x-y*y);
  if(k == 7)return 1/(25*(x*x+y*y) + 1);
  return 0;
}
