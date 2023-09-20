#include "matrinit.h"

#define IA(IS, JS, S) (IA_ij(nx, ny, hx, hy, i, j, (IS), (JS), (S), I, A))



int get_off_diag(int nx, int ny, double hx, double hy,
  int i, int j, int* I, double* A){
    int s = 0;
    if(i < nx)IA(i+1, j, s++);
    if(j > 0)IA(i, j-1, s++);
    if(i>0&&j>0)IA(i-1, j-1, s++);
    if(i > 0)IA(i-1, j, s++);
    if(j < ny)IA(i, j+1, s++);
    if(i<nx&&j<ny)IA(i+1, j+1, s++);
    return s;
}

int get_len_msr_off_diag(int nx, int ny){
  double hx = 0, hy = 0; int i, j, res = 0;
  for(i = 0; i < nx; i++){
    for(j = 0; j < ny; j++){
      res += get_off_diag(nx, ny, hx, hy, i, j);
    }
  }
  return res;
}

int fill_I(int nx, int ny, int* I){
  int i, j, l, r;
  int N = (nx+1)*(ny+1);
  double hx = 0, hy = 0;
  r = N+1;
  for(l = 0; l < N; l++){
    l2ij(nx, ny, i, j, l);
    int s = get_off_diag(nx, ny, hx, hy, i, j);
    I[l]=r;
    r+=s;
  }
  I[l]=r;
  return r;//r=N+1+get_len_msr;
}

int fill_IA(int nx, int ny, double hx, double hy, int* I, double* A, int p, int k){
  //fill_I уже вызвана
  int i, j, l, l1, l2, N=(nx+1)*(ny+1), s = 0, r, t, err = 0, len = 0;
  thread_rows(N, p, k, l1, l2);
  for(l = l1; l<l2; l++){
    l2ij(nx, ny, i, j, l);
    if(get_diag(nx, ny, hx, hy, i, j, I, A+0)!=0){
      err = -1; break;
    }
    r = I[l];
    s = I[l+1]-I[l];
    t = get_off_diag(nx, ny, hx, hy, i, j, I+r, A+r);
    if(s != t){err = -2; break;}
    len += s;
  }
  reduce_sum_int(p, &err, 1);
  if(err<0)return -1;
  reduce_sum_int(p, &len, 1);
  if(I[N]!= (N+1)+len) return -2;
  return 0;
}

int get_diag(int nx, int ny, double hx, double hy, int i, int j, int* /*I*/,
  double* A){
  int l;
  ij2l(nx, ny, i, j, l);
  return IA_ij(nx, ny, hx, hy, i, j, i, j, l/*s*/, nullptr, A);
}


int IA_ij(int nx, int ny, double hx, double hy, int i, int j, int is, int js,
  int s, int* I, double* A){
    int l, ls;
    ij2l(nx, ny, i, j, l);
    ij2l(nx, ny, is, js, ls);
    if(I)I[s]=ls;
    if(A){
      if(l == ls){
        A[s] = (((i < nx && j > 0)?1:0) + ((i > 0 && j > 0)?2:0) +
        ((i < nx && j < ny)?2:0) + ((i > 0 && j < ny)?1:0)) * hx*hy/12;
      }
      else{
        A[s] = (((is == i+1 && js == j)?((j < ny ?1:0) + (j > 0?1:0)):0) +
        ((is == i && js == j-1)?((i < nx ?1:0) + (i > 0?1:0)):0) +
        ((is == i-1 && js == j-1)?2:0) +
        ((is == i-1 && js == j)?((j > 0 ?1:0) + (j < ny?1:0)):0) +
        ((is == i && js == j+1)?((i > 0 ?1:0) + (i < nx?1:0)):0) +
        ((is == i+1 && js == j+1)?2:0)) * hx*hy/24;
      }
      return 0;
    }
    return 1;
  }

#define F(I,J) f(x0+(I)*hx, y0+(J)*hy)

int fill_b(int nx, int ny, double hx, double hy, double x0, double y0,
  double (*f)(double, double), double* b, int p, int k, int err_p, double err_f){
  int n = (nx+1)*(ny+1);
  int l, l1, l2, l0;
  thread_rows(n, p, k, l1, l2);
  ij2l(nx, ny, nx/2, ny/2, l0);
  for(l = l1; l < l2; l++){
    b[l] = F_ij(nx, ny, hx, hy, x0, y0, f, l) + (l == l0?err_p*err_f:0);
  }
  return 0;
}


double F_ij (int nx, int ny, double hx, double hy, double x0, double y0,
  double (*f)(double, double), int l)
{
	int i = 0, j = 0;
	l2ij (nx, ny, i, j, l);
	return (
	       (i < nx && j > 0 ? (2 * F(i,j) + F(i+1,j) + F(i,j-1)) : 0) +
	       (i > 0 && j > 0 ? (4 * F(i,j) + F(i,j-1) + 2 * F(i-1,j-1)+ F(i-1,j)) : 0) +
	       (i > 0 && j < ny ? (2 * F(i,j) + F(i-1,j) + F(i,j+1)) : 0) +
	       (i < nx && j < ny ? (4 * F(i,j) + F(i,j+1) + 2 * F(i+1,j+1) + F(i+1,j)) : 0)
	       ) * hx * hy / 24;
}



/*
double F_ij(int nx, int ny, double hx, double hy, double x0, double y0,
   double (*f)(double, double), int l){
     int i, j;
     l2ij(nx, ny, i, j, l);
     return ( ( i<nx && j>0 ? 6*F(i, j) + 10*F(i, j-0.5) + F(i+1, j) +
     4*F(i+0.5, j-0.5) + F(i, j-1) + 10*F(i, j-0.5) : 0) +
     ( i>0 && j>0 ? 12*F(i, j) + 10*F(i, j-0.5) + F(i, j-1) + 4*F(i-0.5, j-1)
     + 2*F(i-1, j-1) + 20*F(i-0.5, j-0.5) + 4*F(i-1, j-0.5) + F(i-1, j) +
     10*F(i, j-0.5) : 0 ) +
     ( i>0 && j<ny ? 6*F(i, j) + 10*F(i-0.5, j) + F(i-1, j) +
     4*F(i-0.5, j+0.5) + F(i, j+1) + 10*F(i, j+0.5) : 0) +
     ( i<nx && j<ny ? 12*F(i, j) + 10*F(i, j+0.5) + F(i, j+1) + 4*F(i+0.5, j+1)
     + 2*F(i+1, j+1) + 20*F(i+0.5, j+0.5) + 4*F(i+1, j+0.5) + F(i+1, j) +
     10*F(i+0.5, j) : 0 ) ) * hx * hy / 192;
}
*/
