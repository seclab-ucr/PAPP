#ifndef _UTILS_X86_H_
#define _UTILS_X86_H_

#include "../types.h"
#include "../mem_alloc.h"

#define MFENCE() asm volatile ( "mfence;")
// #define MFENCE() asm volatile ( "xor %eax, %eax; cpuid; xor %eax, %eax; cpuid; mfence" );
#define CL_FLUSH(X) asm volatile ("mfence; clflush (%0); mfence" :: "r" (X))

static void setup_clflush() {
    // Don't need to do anything for x86
    return;
}

static void flushLine(struct mem_block *m, int pidx, int sidx, int lidx) {
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    int point = (pidx * n_points_per_page) + (sidx * n_points_per_set) + lidx;
    CL_FLUSH(m->accessPointsByWays[point]);
}

static void __flushMem(struct mem_block *m, int n_lines) {
    int i;
    char read = 0;
    char * addr;
    for (i = 0; i < n_lines; i++) {
        addr = m->accessPointsByWays[i];
        CL_FLUSH(addr);
    }
}


void __clear_eviction_set(struct mem_block * m, int page_idx) {
    int set_idx;
    int line_idx;
    int point;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    for (set_idx = 0; set_idx < N_LINES_PER_PAGE; set_idx++) {
        for (line_idx = 0; line_idx < (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); line_idx++) {
            point = (page_idx * n_points_per_page) + (set_idx * n_points_per_set) + line_idx;
            CL_FLUSH(m->accessPointsByWays[point]);
        }
    }
}

inline __attribute__((always_inline)) uint64_t rdtsc_bgn() {
  REGISTER a, d;
  asm volatile ("mfence\n\t"
    "RDTSCP\n\t"
    "mov %%rdx, %0\n\t"
    "mov %%rax, %1\n\t"
    "xor %%rax, %%rax\n\t"
    "CPUID\n\t"
    : "=r" (d), "=r" (a)
    :
    : "%rax", "%rbx", "%rcx", "%rdx");
  uint64_t r = d;
  r = (r<<32) | (uint64_t)a;
  return r;
}

inline __attribute__((always_inline)) uint64_t rdtsc_end() {
  REGISTER a, d;
  asm volatile(
    "xor %%rax, %%rax\n\t"
    "CPUID\n\t"
    "RDTSCP\n\t"
    "mov %%rdx, %0\n\t"
    "mov %%rax, %1\n\t"
    "mfence\n\t"
    : "=r" (d), "=r" (a)
    :
    : "%rax", "%rbx", "%rcx", "%rdx");
   uint64_t r = d;
   r = (r<<32) | (uint64_t)a;
   return r;
}

#endif
