#ifndef _UTILS_ARM_H_
#define _UTILS_ARM_H_

#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

#include "../types.h"
#include "../mem_alloc.h"

static struct mem_block * m_clflush;
void setup_clflush() {
    // Setup eviction set for cl flush
    m_clflush = malloc(sizeof(struct mem_block));
    *m_clflush = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_SLOW | LINK_MODE_EXTRA_PRIME); 
    // printMemPages(m_clflush);
    // Set Link
    int i;
    for (i = 0; i < m_clflush->numCacheLines - 1; i++) {
        PNP_ADDR_NEXT(m_clflush->accessPointsByWays[i]) = m_clflush->accessPointsByWays[i+1];
    }
    PNP_ADDR_NEXT(m_clflush->accessPointsByWays[m_clflush->numCacheLines - 1]) = m_clflush->accessPointsByWays[0];
}

static void flushLine(struct mem_block *m, int pidx, int sidx, int lidx) {
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    int _lidx;
    int point;
    int i, j;
    // PNP_ADDR addr = m_clflush->accessPointsByWays[(pidx * n_points_per_page) + (sidx * n_points_per_set)];
   
    for (j = 0; j < 3; j++) {
    for (_lidx = 0; _lidx < n_points_per_set; _lidx++) {
        point = (pidx * n_points_per_page) + (sidx * n_points_per_set) + _lidx;
        // fprintf(stderr, "%d %llx\n", point, m_clflush->accessPointsByWays[point]);
        LOAD(m_clflush->accessPointsByWays[point]);
        // addr = PNP_ADDR_NEXT(addr);
    }
    }
}

static void __flushMem(struct mem_block *m, int n_lines) {
    int i,j;
    char read = 0;
    char * addr;
    for (j = 0; j < 5; j++) {
    for (i = 0; i < m_clflush->numCacheLines; i++) {
        addr = m_clflush->accessPointsByWays[i];
        LOAD(addr);
    }
    }
}

#ifdef ENVIRONMENT_64_BIT
#define MFENCE() asm volatile ("DSB SY; ISB")
#define CL_FLUSH(X) asm volatile ("DC CIVAC, %0" :: "r" (X))

#else // ENVIRONMENT_64_BIT
#define MFENCE() asm volatile ("DSB; ISB")
#define CL_FLUSH(X) asm volatile ("MCR p15, 0, %0, c7, c6, 1" :: "r" (X))

#endif

void __clear_eviction_set(struct mem_block * m, int page_idx) {
    int set_idx;
    int line_idx;
    int point;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    for (set_idx = 0; set_idx < N_LINES_PER_PAGE; set_idx++) {
        flushLine(m, page_idx, set_idx, 0);
    }
}

#endif
