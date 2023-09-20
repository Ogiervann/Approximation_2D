#ifndef RESIDUALS_H
#define RESIDUALS_H

#include "basic_funcs.h"


double Pf(double *X, double x, double y, int nx, int ny, double a, double c,
  double hx, double hy);
double find_r1(double *X, double (*f)(double, double), int nx, int ny,
  double hx, double hy, double x0, double y0, int p, int k);
double find_r2(double *X, double (*f)(double, double), int nx, int ny,
  double hx, double hy, double x0, double y0, int p, int k);
double find_r3(double* x, double (*f)(double, double), int nx, int ny,
  double hx, double hy, double x0, double y0, int p, int k);
double find_r4(double* x, double (*f)(double, double), int nx, int ny,
  double hx, double hy, double x0, double y0, int p, int k);

#endif
