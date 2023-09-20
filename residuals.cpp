#include "residuals.h"

double Pf(double *X, double x, double y, int nx, int ny, double a, double c,
          double hx, double hy)
{
  if(x < a) x = a;
  if(y < c) y = c;
  int i = (x - a) / hx, j = (y - c) / hy, l1, l2, l3;
if(i >= nx) i = nx-1;
if(j >= ny) j = ny-1;
if (y - hy * (x - (a + i * hx)) / hx - (c + j * hy) <= 0) {
    ij2l(nx, ny, i + 1, j + 1, l1);
    ij2l(nx, ny, i + 1, j, l2);
    ij2l(nx, ny, i, j, l3);
    return X[l1] * (y - c - j * hy) / hy +
        X[l2] * (1. + (x - a - (i + 1) * hx) / hx - (y - c - j * hy) / hy) -
        X[l3] * (x - a - (i + 1) * hx) / hx;
}

ij2l(nx, ny, i + 1, j + 1, l1);
ij2l(nx, ny, i, j + 1, l2);
ij2l(nx, ny, i, j, l3);
return X[l1] * (x - a - i * hx) / hx +
    X[l2] * (1. - (x - a - i * hx) / hx + (y - c - (j + 1) * hy) / hy) -
    X[l3] * (y - c - (j + 1) * hy) / hy;
}




double find_r1(double *X, double (*f)(double, double), int nx, int ny,
                 double hx, double hy, double x0, double y0, int p, int k)
{
  //||f-Pf||_C mass centre
  int l, l1, l2;
  int N = (nx + 1) * (ny + 1);
  int i, j;
  double max = 0, score;
  thread_rows(N, p, k, l1, l2);
  for (l = l1; l < l2; l++)
  {
    l2ij(nx, ny, i, j, l);
    if (i == nx || j == ny)
      continue;
    score = fabs(f(x0 + (i + 2/3) * hx, y0 + (j + 1/3) * hy) -
                 Pf(X, x0 + (i + 2/3) * hx, y0 + (j + 1/3) * hy, nx, ny,
                    x0, y0, hx, hy));
    if (score > max)
      max = score;
    score = fabs(f(x0 + (i + 1. / 3) * hx, y0 + (j + 2. / 3) * hy) -
                 Pf(X, x0 + (i + 1. / 3) * hx, y0 + (j + 2. / 3) * hy, nx, ny,
                    x0, y0, hx, hy));
    if (score > max)
      max = score;
  }
  return max;
}

double find_r2(double *X, double (*f)(double, double), int nx, int ny,
                 double hx, double hy, double x0, double y0, int p, int k)
{
  // ||f-Pf||_L1 mass centr

  int l, l1, l2;
  int N = (nx + 1) * (ny + 1);
  int i, j;
  double score = 0;
  thread_rows(N, p, k, l1, l2);
  for (l = l1; l < l2; l++)
  {
    l2ij(nx, ny, i, j, l);
    if (i == nx || j == ny)
      continue;
    score += fabs(f(x0 + (i + 1. / 3) * hx, y0 + (j + 2. / 3) * hy) -
                  Pf(X, x0 + (i + 1. / 3) * hx, y0 + (j + 2. / 3) * hy, nx, ny,
                     x0, y0, hx, hy)) *
             hx * hy / 2;
    score += fabs(f(x0 + (i + 2. / 3) * hx, y0 + (j + 1. / 3) * hy) -
                  Pf(X, x0 + (i + 2. / 3) * hx, y0 + (j + 1. / 3) * hy, nx, ny,
                     x0, y0, hx, hy)) *
             hx * hy / 2;
  }
  return score;
}

double find_r3(double *X, double (*f)(double, double), int nx, int ny,
                 double hx, double hy, double x0, double y0, int p, int k)
{
  // ||f-Pf||_C nodes
  int i, j, j1, j2;
  double max = 0, score;
  thread_rows(ny, p, k, j1, j2);
  for (j = j1; j < j2; j++)
    for (i = 0; i < nx; i++)
    {
      score = fabs(f(x0 + i * hx, y0 + j * hy) -
                   Pf(X, x0 + i * hx, y0 + j * hy, nx, ny, x0, y0, hx, hy));
      if (score > max)
        max = score;
    }
  return max;
}

double find_r4(double *X, double (*f)(double, double), int nx, int ny,
                 double hx, double hy, double x0, double y0, int p, int k)
{
  // ||f-Pf||_L nodes
  int i, j, j1, j2;
  double score = 0;
  thread_rows(ny, p, k, j1, j2);
  for (j = j1; j < j2; j++)
    for (i = 0; i < nx; i++)
    {
      score = fabs(f(x0 + i * hx, y0 + j * hy) -
                    Pf(X, x0 + i * hx, y0 + j * hy, nx, ny, x0, y0, hx, hy)) *
               hx * hy;
    }
  return score;
}
