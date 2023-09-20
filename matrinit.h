#ifndef MATRINIT_H
#define MATRINIT_H

#include "basic_funcs.h"


int IA_ij(int nx, int ny, double hx, double hy, int i, int j, int is, int js,
  int s, int* I = nullptr, double* A = nullptr);


int get_off_diag(int nx, int ny, double hx, double hy, int i, int j,
  int* I = nullptr, double* A = nullptr);

int get_len_msr_off_diag(int nx, int ny);

int fill_I(int nx, int ny, int* I);

int fill_IA(int nx, int ny, double hx, double hy, int* I, double* A, int p, int k);

int fill_b(int nx, int ny, double hx, double hy, double x0, double y0,
  double (*f)(double, double), double* b, int p, int k, int err_p, double err_f);

int get_diag(int nx, int ny, double hx, double hy, int i, int j,
  int* /*I*/, double* A);

double F_ij(int nx, int ny, double hx, double hy, double x0, double y0,
   double (*f)(double, double), int l);



#endif
