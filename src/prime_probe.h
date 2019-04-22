#ifndef PRIME_PROBE_H
#define PRIME_PROBE_H

#include <time.h>

#include "mem_alloc.h"
#include "global.h"

#define VERBOSE_PNP 1

#define PROBE_N_TIME(ADDR, T0, T1) { \
    RDTSC(T0); \
    ADDR = PNP_ADDR_NEXT(addr); \
    RDTSCP(T1); \
}


static void primeAndProbeTraditional(PNP_ADDR head, LOG_ENTRY *result, unsigned int page_mask, int lines_per_set, int num_sets) {
    int i, j, k;
    uint64_t ts0, ts1;
    PNP_ADDR pnp_ptr = head;
    for (k = 0; k < NUM_PAGES; k++) {
        if (((1 << k) & page_mask) == 0)
            continue;
        for (j = 0; j < num_sets; j++) {
            RDTSCP(ts0);
            i = 0;
            while (i < N_OF_WAYS) {
                pnp_ptr = PNP_ADDR_NEXT(pnp_ptr);
                i++;
            }
            RDTSCP(ts1);
            while (i < lines_per_set) {
                pnp_ptr = PNP_ADDR_NEXT(pnp_ptr);
                i++;
            }
            *result = (ts1 - ts0) > THRESHOLD_SET;
            result ++;
        }
    }
}

static void primeAndProbePerPage(PNP_ADDR head, LOG_ENTRY *result, unsigned int page_mask, int lines_per_set, int probe_end, int probe_bgn) {
    int i, j, k;
    uint64_t ts0, ts1;
    PNP_ADDR ptr = head;
    for (j = 0; j < NUM_PAGES; j++) {
        if (((1 << j) & page_mask) == 0)
            continue;
        for (i = 0; i < lines_per_set; i++) {
            RDTSCP(ts0);
            pnp_ptr = PNP_ADDR_NEXT(pnp_ptr);
            RDTSCP(ts1);
            ts0 = ts1 - ts0;
            if (i < probe_end && PNP_I >= probe_bgn) {
                *result = ts0 > THRESHOLD_BYTE;
                result ++;
            }
        }
    }
}

#ifdef TRADITIONAL_PNP
#define PRIME_AND_PROBE_TRADITIONAL(HEAD, RESULT, I, J, K, PTR, TS0, TS1, PAGES, SETS, LINES) \
    for (K = 0; K < PAGES; K++) { \
      for (J = 0; J < SETS; J++) { \
        RDTSCP(TS0); \
        I = 0; \
        while (I < N_OF_WAYS) { \
          PTR = *PTR; \
          I++; \
        } \
        RDTSCP(TS1); \
        while (I < LINES) { \
          PTR = *PTR; \
          I++; \
        } \
        *RESULT = (TS1 - TS0) > THRESHOLD_SET; \
        RESULT ++; \
      } \
    } \

#define PRIME_AND_PROBE_TRADITIONAL_ADAPTIVE(HEAD, RESULT, I, J, K, PTR, TS0, TS1, PAGES, SETS, LINES, VACANTS, VACANTS_PTR) \
    for (K = 0; K < PAGES; K++) { \
      /* fprintf(stderr, "%#10x ", PTR); */\
      for (J = 0; J < SETS; J++) { \
        VACANTS_PTR = VACANTS[K * N_LINES_PER_PAGE + J]; \
        I = VACANTS_PTR; \
        /* if (I > 0 && J == 0) fprintf(stderr, "%#10x %d %d %d\n", PTR, K, J, I); */ \
        RDTSCP(TS0); \
        while (I < N_OF_WAYS) { \
          PTR = *PTR; \
          I++; \
        } \
        RDTSCP(TS1); \
        while (I < LINES) { \
          PTR = *PTR; \
          I++; \
        } \
        *RESULT = (TS1 - TS0) > THRESHOLD_SET - (VACANTS_PTR * THRESHOLD_SET_STEP); \
        RESULT ++; \
      } \
    } \
    /* fprintf(stderr, "\n"); */

#endif

#ifdef TRADITIONAL_PNP
void __PRIME_AND_PROBE_TRADITIONAL_ADAPTIVE() {
    for (PNP_K = PNP_PAGE_OFFSET; PNP_K < PNP_PAGE_OFFSET + PNP_NUM_PAGES; PNP_K++) {
      // fprintf(stderr, "%#10x ", pnp_ptr);
      for (PNP_J = 0; PNP_J < PNP_NUM_SETS; PNP_J++) {
        // fprintf(stderr, "%#10x\n", pnp_ptr);
        PNP_VACANTS_PTR = adaptive_vacant_sequence[PNP_K * N_LINES_PER_PAGE + PNP_J];
        PNP_I = PNP_VACANTS_PTR;
        /* if (I > 0 && J == 0) fprintf(stderr, "%#10x %d %d %d\n", PTR, K, J, I); */
        RDTSCP(PNP_TS0);
        while (PNP_I < N_OF_WAYS) {
          pnp_ptr = PNP_ADDR_NEXT(pnp_ptr);
          PNP_I++;
        }
        RDTSCP(PNP_TS1);
        PNP_I = 0;
        while (PNP_I < N_OF_WAYS) {
          pnp_ptr = PNP_ADDR_NEXT(pnp_ptr);
          PNP_I++;
        }
        *pnp_log_ptr = (PNP_TS1 - PNP_TS0) > THRESHOLD_SET - (PNP_VACANTS_PTR * THRESHOLD_SET_VACANT_STEP);
        // PNP_TS2 = (PNP_TS1 - PNP_TS0) - (THRESHOLD_SET - (PNP_VACANTS_PTR * THRESHOLD_SET_VACANT_STEP));
        // if (PNP_TS2 > 0)
        //     *pnp_log_ptr = PNP_TS2 / THRESHOLD_SET_STEP + 1;
        // else
        //     *pnp_log_ptr = 0;
        // fprintf(stderr, "%d %d %d %hhu\n", PNP_K, PNP_J, PNP_TS2, *pnp_log_ptr);
        pnp_log_ptr ++;
      }
    }
}
#else
void __PRIME_AND_PROBE_PER_PAGE() {
#ifdef ANDROID
    unsigned int ts0, ts1;
#endif
    // asm volatile ( "push %rbx" );
    // for (PNP_J = PNP_PAGE_OFFSET; PNP_J < PNP_PAGE_OFFSET + PNP_NUM_PAGES; PNP_J++) {
    for (PNP_J = 0; PNP_J < NUM_PAGES; PNP_J++) {
      if (((1 << PNP_J) & PNP_PAGE_MASK) == 0)
        continue;
      for (PNP_I = 0; PNP_I < adaptive_total_len[PNP_J]; PNP_I++) {
        /*
        asm volatile (
                  "mov %[addr_in], %%rbx;"
                  "rdtscp;"
                  "mov %%eax, %%edi;"
                  "mov (%%rbx), %%rbx;"
                  "rdtscp;"
                  "sub %%edi, %%eax;"
                  "mov %%eax, %[ts_out];"
                  "mov %%rbx, %[addr_out];"
                  : [addr_out] "=r" (pnp_ptr),
                    [ts_out] "=r" (PNP_TS0)
                  : [addr_in] "r" (pnp_ptr)
        );
        */
        // fprintf(stderr, "%d\t%d\t0x%llx -> 0x%llx\n", PNP_I, adaptive_total_len[PNP_J], pnp_ptr, PNP_ADDR_NEXT(pnp_ptr));
#ifdef ANDROID
        RDTSCP(ts0);
        pnp_ptr = PNP_ADDR_NEXT(pnp_ptr);
        RDTSCP(ts1);
        ts0 = ts1 - ts0;
        if (PNP_I < PROBE_END && PNP_I >= PROBE_BGN) {
            *pnp_log_ptr = ts0 > THRESHOLD_BYTE;
            pnp_log_ptr ++;
        }
#else
        RDTSCP(PNP_TS0);
        pnp_ptr = PNP_ADDR_NEXT(pnp_ptr);
        RDTSCP(PNP_TS1);
        PNP_TS0 = PNP_TS1 - PNP_TS0;
        if (PNP_I < PROBE_END && PNP_I >= PROBE_BGN) {
            *pnp_log_ptr = PNP_TS0 > THRESHOLD_BYTE;
            pnp_log_ptr ++;
        }
#endif
      }
    }
    // asm volatile ( "pop %rbx" );
}
#endif

static void prime(struct mem_block * m) {
    volatile register int i = m->numCacheLines;
    uint64_t * addr = m->accessPointsByWays;
    while (i > 0) {
        *addr = *addr + i;
        addr = addr + 1;
        i = i - 1;
    }
    return;
}

#endif
