#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#ifdef ANDROID
#define FPRINTF_ERR __android_log_print(ANDROID_LOG_DEBUG, "NativeTest.Cheater",
#define FPRINTF_OUT __android_log_print(ANDROID_LOG_DEBUG, "NativeTest.Cheater",
#else
#define FPRINTF_ERR fprintf(stderr,
#define FPRINTF_OUT fprintf(stdout,
#endif

#include "../global.h"
#include "phyaddr.h"
#include "tsc.h"
#include "debug_tools.h"
// #include "scheduler_tools.h"

/*
#define MEM_BUS_RELAX() \
        asm volatile ("mov %eax, %edi;"); \
        asm volatile ("xor %eax, %eax; cpuid; xor %eax, %eax; cpuid; xor %eax, %eax; cpuid; xor %eax, %eax; cpuid;"); \
        asm volatile ("mov %edi, %eax");
        */

// A utility function to swap to integers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// A function to generate a random permutation of arr[]
void randomizeArray( int * arr, int n ) {
    // Use a different seed value so that we don't get same
    // result each time we run this program
    srand ( time(NULL) );

    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = n-1; i > 0; i--) {
        // Pick a random index from 0 to i
        int j = rand() % (i+1);

        // Swap arr[i] with the element at random index
        swap(&arr[i], &arr[j]);
    }
}

void printGBufs() {
    FILE * fp = fopen("/proc/self/maps", "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    if (fp == NULL)
        return;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(strstr(line, "/dev/dri/card0") != NULL) {
            FPRINTF_ERR "%s", line);
        }
    }
    fclose(fp);
}

void setAffinity(int affinity) {
	int result;
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(affinity, &mask);
	result = sched_setaffinity(0, sizeof(mask), &mask);
	// FPRINTF_ERR "Affinity set for current process to %d. Status: %d\n", affinity, result);
}

int run_function(int core, void (*func)(void * x), void * arg) {
    pid_t child = fork();
    if (child < 0) {
        FPRINTF_ERR "fork failed\n");
        exit (EXIT_FAILURE);
    }
    if (child == 0) {
        setAffinity(core);
        func(arg);
        exit(0);
    } else
        return child;
}

static void linkedMemset(PNP_ADDR addr, int size) {
    int num_of_items = (size / sizeof(int*));
    PNP_ADDR bgn = addr;
    PNP_ADDR end = addr + num_of_items;
    PNP_ADDR i = bgn;
    while (i < end - 1) {
        PNP_ADDR_NEXT(i) = i + sizeof(PNP_ADDR);
        i++;
    }
}

static int linkedMemAccess(PNP_ADDR addr, int size) {
    int num_of_items = (size / sizeof(int*));
    int i = 0;
    PNP_ADDR read = addr;
    while (i < num_of_items - 1) {
        // FPRINTF_ERR "%d\t%#10x -> %#10x\n", i, read, *read);
        read = PNP_ADDR_NEXT(read);
        i++;
    }
    return read;
}

/*
 * itoa
 */
#define INT_DIGITS 19           /* enough for 64 bit integer */
static char *itoa(int i) {
    /* Room for INT_DIGITS digits, - and '\0' */
    static char buf[INT_DIGITS + 2];
    char *p = buf + INT_DIGITS + 1; /* points to terminating '\0' */
    if (i >= 0) {
        do {
            *--p = '0' + (i % 10);
            i /= 10;
        } while (i != 0);
        return p;
    } else { /* i < 0 */
        do {
            *--p = '0' - (i % 10);
            i /= 10;
        } while (i != 0);
        *--p = '-';
    }
    return p;
}

void markTest(const char * msg) {
    FILE * f = fopen("/mnt/sdcard/signal_test_begin", "a");
    fprintf(f, msg);
    fprintf(f, "\n");
    fclose(f);
}

static int load_data = 0;
#define LOAD(ADDR) { load_data = load_data ^ *((int*)ADDR); }

#ifdef __ARM__
#include "utils_arm.h"
#else
#include "utils_x86.h"
#endif

void init_utils() {
    fprintf(stderr, "sizeof(PNP_ADDR)\t%d\n", sizeof(PNP_ADDR));
    loadCacheParams();
    tscSetup();
    setup_clflush();
}

static void flushMem(struct mem_block *m) {
    __flushMem(m, m->numCacheLines);
}

static void printArray(int * a, int n) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

#endif
