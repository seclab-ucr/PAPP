#ifndef _SCHEDULER_TOOLS_H_
#define _SCHEDULER_TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#define __USE_GNU
#include <sched.h>
#include <signal.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#ifndef __ARM__
#include <xmmintrin.h>
#else
#include <linux/perf_event.h>
#endif

#include "../global.h"
#include "tsc.h"

/******************************************
 *         PROCESSES AND THREADS          *
 ******************************************/

/* spinlock */
#define WAIT_TO_ADVANCE(x) \
  while((x)>=(*advance_ctr)); \
  x= (*advance_ctr);

struct timespec sleeper = {0, 0};
#define inr_pause() inr_syscall0(__NR_pause)
#define inr_sleep(SEC) sleeper.tv_sec=SEC; inr_syscall2(__NR_nanosleep, &sleeper, NULL)
void set_affinity(int core);
void wakeup() { /* fprintf(stderr, "ALARM\n"); */}
void terminate(int x) { exit(0); }
void set_timer(timer_t * t);
void set_alarm(timer_t t, long nsec);

/* POSIX timer stuff for setting precise alarms */

// #define i_timer_create(RET, CLOCK, SE, T) i_syscall3(RET, __NR_timer_create, CLOCK, SE, T)
// #define i_timer_settime(RET, T, FLAGS, NEW, OLD) i_syscall4(RET, __NR_timer_settime, T, FLAGS, NEW, OLD)

/*
 * every thread in CFS attack creates an alarm timer for waking themselves
 * and each thread arms the timer of the next thread during the attack
 */
void set_timer(timer_t * t) {
  struct sigevent se;
  long ret;
  se.sigev_notify = SIGEV_THREAD_ID;
  se._sigev_un._tid = syscall(SYS_gettid);
  // se.sigev_notify_thread_id = syscall(SYS_gettid);
  se.sigev_signo = SIGALRM;
  if(timer_create(CLOCK_MONOTONIC, &se, t)==-1) {
  //i_timer_create(ret, CLOCK_MONOTONIC, &se, t);
  //if(ret!=0) {
    perror("timer creation error\n");
    exit(EXIT_FAILURE);
  }
}

struct itimerspec its = {{0,0}, {0,0}};

#define SET_ALARM(T, NSEC) do{ \
  long ret; \
  its.it_value.tv_nsec = NSEC; \
  i_timer_settime(ret, T, 0, &its, NULL); \
  if (ret != 0) { \
    FPRINTF_ERR "ret=%ld i=%d (inval=%d, badf=%d), timers=%p timer=%ld\n",ret,i,EINVAL, EBADF,timers, (long)T); \
    errno = -ret; \
    perror("timer settime error\n"); \
    exit(EXIT_FAILURE); \
  } \
} while(0) //"


//  i_timer_settime(ret, T, 0, &its, NULL);
//  if(ret != 0) {

void set_alarm(timer_t t, long nsec) {
  struct itimerspec its;
  long ret;
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = nsec;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  if (timer_settime(t, 0, &its, NULL) == -1) {
  // i_timer_settime(ret, t, 0, &its, NULL);
  // if(ret != 0) {
    perror("timer settime error\n");
    exit(EXIT_FAILURE);
  }
}

/******************************************
 *                 TIMING                 *
 ******************************************/

#if 0
static int fddev = -1;
__attribute__((constructor)) static void init(void) {
    static struct perf_event_attr attr;
    attr.type = PERF_TYPE_HARDWARE;
    attr.config = PERF_COUNT_HW_CPU_CYCLES;
    attr.disabled = 0;
    fddev = syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0);
}

__attribute__((destructor)) static void fini(void) {
    close(fddev);
}

static inline uint64_t readtsc() {
    uint64_t result = 0;
    // if (read(fddev, &result, sizeof(result)) < sizeof(result)) return 0;
    asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(result));
    return result;
}
/*
static struct perf_event_attr perf_attr = {
        .type = PERF_TYPE_HARDWARE,
        .config = PERF_COUNT_HW_CPU_CYCLES,
        .read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING
};
static inline uint64_t readtsc() {
   uint64_t result = 0;
   int fddev = -1;
   fddev = syscall(__NR_perf_event_open, &perf_attr, 0, -1, -1, 0);
   ioctl(fddev, PERF_EVENT_IOC_ENABLE, 0);
   if (read(fddev, &result, sizeof(result)) < sizeof(result))
       return -1;
   close(fddev);
   return result;
}
*/
#endif

#ifdef __ARM__
void busy_wait() {
  uint64_t current=0, prev=0;
  while(prev==0 || current-prev<10000) {
    prev = current;
    current = getTime();
  }
}

/* busy wait for a given time */
#define FDELAY(D) do {\
    register uint64_t now = getTime(); \
    while(getTime() <= now+D) ; \
  } while(0)

#define FDELAY_UNTIL(T) do {\
    while(getTime() <= T) /*_mm_pause()*/; \
  } while(0)

void delay(uint64_t delay) {
  uint64_t now = getTime();
  while(getTime() <= now+delay);
}

#else /* __ARM__ */
void busy_wait() {
  uint64_t current=0, prev=0;
  while(prev==0 || current-prev<10000) {
    prev = current;
    current = readtsc();
  }
}

/* busy wait for a given time */
#define FDELAY(D) do {\
    register uint64_t now = readtsc(); \
    while(readtsc() <= now+D) _mm_pause(); \
  } while(0)

#define FDELAY_UNTIL(T) do {\
    while(readtsc() <= T) /*_mm_pause()*/; \
  } while(0)

void delay(uint64_t delay) {
  uint64_t now = readtsc();
  while(readtsc() <= now+delay);
}

#endif

/******************************************
 *                  MISC                  *
 ******************************************/
/* assign the process to a core */
void set_affinity(int coreid) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(coreid, &mask);
  if(sched_setaffinity( 0, sizeof(mask), &mask ) == -1 ) {
    perror("WARNING: Could not set CPU Affinity, continuing...\n");
  }
}


#endif
