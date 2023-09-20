#ifndef USED_CLASSES_H
#define USED_CLASSES_H
#include <pthread.h>
#include <cmath>

static
double f_0 (double /*x*/, double /*y*/)
{
  return 1;
}

static
double f_1 (double x, double /*y*/)
{
  return x;
}

static
double f_2 (double /*x*/, double y)
{
  return y;
}

static
double f_3 (double x, double y)
{
  return x + y;
}

static
double f_4 (double x, double y)
{
  return sqrt(x*x+y*y);
}

static
double f_5 (double x, double y)
{
  return x*x+y*y;
}


static
double f_6 (double x, double y)
{
  return exp(x*x-y*y);
}


static
double f_7 (double x, double y)
{
  return 1/(25*(x*x+y*y)+1);
}



class Results{
public:
  int status = -100;
  double alg_time = 0;
  double res_time = 0;
  int it = 0;
  double r1 = -1;
  double r2 = -1;
  double r3 = -1;
  double r4 = -1;
};

class GResults{
public:
  bool in_progress_flag = false;
};

class Args{
public:
  Results *res = nullptr;
  bool* in_progress_flag = nullptr;
  GResults gresults;
  double a = 0; double b = 0;
  double c = 0; double d = 0;
  int nx = 0; int ny = 0;
  double err = 0;
  int err_p = 0;
  double eps = 0;
  int maxit = 0; int func_id = 0;
  double (*f)(double, double) = nullptr;
  int k = 0; int p = 0;
  double *A = nullptr;
  int *I = nullptr;
  double *B = nullptr;
  double *x = nullptr;
  double *r = nullptr;
  double *u = nullptr;
  double *v = nullptr;
  pthread_t tid = -1;

  void set_func(){
    if(func_id == 0)f = f_0;
    if(func_id == 1)f = f_1;
    if(func_id == 2)f = f_2;
    if(func_id == 3)f = f_3;
    if(func_id == 4)f = f_4;
    if(func_id == 5)f = f_5;
    if(func_id == 6)f = f_6;
    if(func_id == 7)f = f_7;
  }
};

#endif
