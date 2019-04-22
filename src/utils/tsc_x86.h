#ifndef _TSC_X86_H_
#define _TSC_X86_H_

// #define RDTSC(X) asm volatile ( "mov %%ebx, %%edi; xor %%eax, %%eax; cpuid; rdtsc; mov %%edi, %%ebx" : "=a" (X));

static inline uint64_t readtsc() {
    register unsigned int hi, lo;
    asm volatile ( "rdtscp;" : "=d" (hi) , "=a" (lo));
    return  ( ((uint64_t) hi << 32) | lo );
}

#define TSC_TEST_ROUNDS 100
static void tscSetup() {
    THRESHOLD_BYTE = 40;
}

#endif
