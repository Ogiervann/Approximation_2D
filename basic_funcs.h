#ifndef BASIC_FUNCS_H
#define BASIC_FUNCS_H
#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cmath>

void ij2l(int nx, int /*ny*/, int i, int j, int &l);
void l2ij(int nx, int /*ny*/, int &i, int &j, int l);

int get_len_msr(int nx, int ny);

void thread_rows(int n, int p, int k, int&i1, int &i2);

void reduce_sum(int p, double* a = nullptr, int n = 0);
void reduce_sum_int(int p, int* a = nullptr, int n = 0);

double get_full_time();
double get_CPU_time();



double f(double x, double y, int k);

#endif
