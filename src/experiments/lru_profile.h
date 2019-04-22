#ifndef _LRU_PROFILE_H_
#define _LRU_PROFILE_H_

#include <sys/mman.h>

#include "../global.h"
#include "../mem_alloc.h"
#include "../prime_probe.h"

static char lru_log_singleset[64] = "lru_singleset.log";

static char lru_log_singleset_android[64] = "/mnt/sdcard/lru_singleset.log";

#ifdef __ARM__
void _lru_clear_eviction_set(struct mem_block * m, int page_idx) {
    int set_idx;
    int line_idx;
    int point;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    for (set_idx = 0; set_idx < N_LINES_PER_PAGE; set_idx++) {
        flushLine(m, page_idx, set_idx, 0);
    }
}

#else

void _lru_clear_eviction_set(struct mem_block * m, int page_idx) {
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

#endif

float lru_profile_multiset(struct mem_block * m, struct mem_block * victim_m, int page_idx, uint64_t set_mask, int probe_set, int probe_line, int rounds) {
    // Set link according to order for each page
    int i, j;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    int n_points_per_set_prime = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    PNP_ADDR bgn = 0, prev = 0;
    int prime_point;
    int sets_count = 0;
    int set_idx;

    // Prime sequence
    for (set_idx = 0; set_idx < N_LINES_PER_PAGE; set_idx++) {
        if ((((uint64_t)1 << set_idx) & set_mask) == 0)
            continue;
        else
            sets_count += 1;
        for (i = 0; i < n_points_per_set_prime; i++) {
            prime_point = (page_idx * n_points_per_page) + (set_idx * n_points_per_set) + i;
            // fprintf(stderr, "*%d %d %d %d %#llx*\n", page_idx, set_idx, i, prime_point, m->accessPointsByWays[prime_point]);
            if (bgn == 0) {
                bgn = m->accessPointsByWays[prime_point];
                prev = bgn;
            } else {
                PNP_ADDR_NEXT(prev) =  m->accessPointsByWays[prime_point];
                prev = PNP_ADDR_NEXT(prev);
            }
        }
    }

    // Probe sequence
    int probe_point = (page_idx * n_points_per_page) + (probe_set * n_points_per_set) + probe_line;
    // fprintf(stderr, "#%d %d %d %d %#llx#\n", page_idx, probe_set, probe_line, probe_point, m->accessPointsByWays[probe_point]);
    PNP_ADDR_NEXT(prev) = m->accessPointsByWays[probe_point] + 8;
    prev = PNP_ADDR_NEXT(prev);
    m->linkHead = bgn;

    // Test
    uint64_t ts0, ts1;
    int r;
    float ret_novic = 0.0;
    float ret_vic = 0.0;
    PNP_ADDR addr;
    PNP_ADDR victim_addr = victim_m->accessPointsByWays[page_idx * n_points_per_page];
    PNP_ADDR victim_addr_set = victim_m->accessPointsByWays[page_idx * n_points_per_page + set_idx * n_points_per_set];
    PNP_ADDR victim_addrs[64];
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        victim_addrs[i] = victim_m->accessPointsByWays[page_idx * n_points_per_page + i * n_points_per_set];
    }

    // No victim
    for (r = 0; r < rounds; r++) {
        addr = m->linkHead;
        _lru_clear_eviction_set(m, page_idx);
        MFENCE();

        for (i = 0; i < n_points_per_set_prime * sets_count; i++) {
            addr = PNP_ADDR_NEXT(addr);
        }

        RDTSCP(ts0);
        addr = PNP_ADDR_NEXT(addr);
        RDTSCP(ts1);

        // ret_novic += (ts1 - ts0) / rounds;
        if (ts1 - ts0 < THRESHOLD_BYTE) ret_novic += 1.0 / rounds;
    }

    // With victim
    for (r = 0; r < rounds; r++) {
        addr = m->linkHead;
        _lru_clear_eviction_set(m, page_idx);
        _lru_clear_eviction_set(victim_m, page_idx);
        MFENCE();

        for (i = 0; i < n_points_per_set_prime * sets_count; i++) {
            addr = PNP_ADDR_NEXT(addr);
        }

        MFENCE();
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            LOAD(victim_addrs[i]);
        }
        MFENCE();

        RDTSCP(ts0);
        addr = PNP_ADDR_NEXT(addr);
        RDTSCP(ts1);

        // ret_vic += (ts1 - ts0) / rounds;
        if (ts1 - ts0 < THRESHOLD_BYTE) ret_vic += 1.0 / rounds;
    }
    fprintf(stderr, "%d, %d:\t%.02f : %.02f\n", probe_set, probe_line, ret_novic, ret_vic);

    return ret_novic - ret_vic;
}

void lru_profile_singlesetSimulation(struct mem_block * m, struct mem_block * victim_m, int page_idx, int rounds) {
    float result;
    int set_idx = 0;
    int probe_set = 0;
    int probe_line = 0;
    uint64_t set_mask = 0;

    FILE * fout = fopen(lru_log_singleset, "w+");
    if (fout == 0)
        fout = fopen(lru_log_singleset_android, "w+");
    if (fout == 0) {
        fprintf(stderr, "ERROR. Cannot open log file\n");
        exit(1);
    }

    for (set_idx = 0; set_idx < N_LINES_PER_PAGE; set_idx ++) {
        // set_mask = set_mask | ((uint64_t)1 << set_idx);
        set_mask = ((uint64_t)1 << set_idx);
        fprintf(fout, "-------------------------------\n");
        fprintf(stderr, "0x%llx\n", set_mask);
        fprintf(fout, "0x%llx\n", set_mask);
        probe_set = set_idx;
        // for (probe_set = 0; probe_set < N_LINES_PER_PAGE; probe_set++) {
            fprintf(fout, "%d: ", probe_set);
            for (probe_line = 0; probe_line < (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); probe_line++) {
                result = 0.0;
                // for (page_idx = 0; page_idx < NUM_PAGES; page_idx ++) {
                result += lru_profile_multiset(m, victim_m, page_idx, set_mask, probe_set, probe_line, rounds);
                // }
                // result = result / NUM_PAGES;
                fprintf(fout, "%.02f ", result);
            }
            fprintf(fout, "\n", result);
            fflush(fout);
        // }
    }

    fclose(fout);
}

void lru_profile_main() {
    struct mem_block m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_SLOW | LINK_MODE_EXTRA_PRIME);
    struct mem_block victim_m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_SLOW | LINK_MODE_EXTRA_PRIME);

    lru_profile_singlesetSimulation(&m, &victim_m, 0, 100);

    freeMem(&m);
    freeMem(&victim_m);
}

#endif
