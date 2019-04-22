#ifndef _TSC_H_
#define _TSC_H_

#ifdef __I386__
#include "tsc_x86.h"
#else
#include "tsc_arm.h"
#endif

#define RDTSC(X) X=readtsc()
#define RDTSCP(X) X=readtsc()

#endif
