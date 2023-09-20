#ifndef MY_THREAD_H
#define MY_THREAD_H
#include <pthread.h>
#include "used_classes.h"
#include <math.h>
#include <cstdio>
#include <ctime>
#include <cstring>
#include "basic_funcs.h"
#include "operations.h"
#include "residuals.h"
#include "msr_funcs.h"
#include "matrinit.h"


void* thread_func(void* ptr);

#endif
