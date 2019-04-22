#ifndef PREFETCH_PROFILE_H
#define PREFETCH_PROFILE_H

#include <sys/mman.h>

#include "../global.h"
#include "../mem_alloc.h"
#include "../prime_probe.h"

static char log_traditional[64] = "traditional.log";
static char log_singleline[64] = "singleline.log";
static char log_singleset[64] = "singleset.log";
static char log_multiset[64] = "multiset.log";

static char log_traditional_android[64] = "/mnt/sdcard/traditional.log";
static char log_singleline_android[64] = "/mnt/sdcard/singleline.log";
static char log_singleset_android[64] = "/mnt/sdcard/singleset.log";
static char log_multiset_android[64] = "/mnt/sdcard/multiset.log";

float prefetch_profile_singleline(struct mem_block * m, int page_idx, int prime_set, int prime_line, int probe_set, int probe_line, int rounds) {
    // Set link according to order for each page
    int i;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    int n_points_per_set_prime = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    PNP_ADDR bgn = 0, prev = 0;

    // Prime sequence
    int prime_point = (page_idx * n_points_per_page) + (prime_set * n_points_per_set) + prime_line;
    bgn = m->accessPointsByWays[prime_point];
    prev = bgn;

    // Probe sequence
    int probe_point = (page_idx * n_points_per_page) + (probe_set * n_points_per_set) + probe_line;
    PNP_ADDR_NEXT(prev) = m->accessPointsByWays[probe_point] + 8;
    prev = PNP_ADDR_NEXT(prev);

    // Fin
    PNP_ADDR_NEXT(bgn);
    m->linkHead = bgn;

    // Test
    uint64_t ts0, ts1;
    int r;
    float ret = 0.0;
    PNP_ADDR addr;
    for (r = 0; r < rounds; r++) {
        addr = m->linkHead;
        __clear_eviction_set(m, page_idx);
        MFENCE();
        addr = PNP_ADDR_NEXT(addr);

        RDTSCP(ts0);
        addr = PNP_ADDR_NEXT(addr);
        RDTSCP(ts1);

        if (ts1 - ts0 < THRESHOLD_BYTE)
            ret += 1.0 / rounds;
    }
    return ret;
}

float prefetch_profile_traditionalMultiset(struct mem_block * m, int page_idx, uint64_t set_mask, int probe_set, int probe_line, int rounds) {
    // Set link according to order for each page
    int i;
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
    float ret = 0.0;
    PNP_ADDR addr;
    for (r = 0; r < rounds; r++) {
        addr = m->linkHead;
        __clear_eviction_set(m, page_idx);
        MFENCE();

        for (i = 0; i < n_points_per_set_prime * sets_count; i++) {
            addr = PNP_ADDR_NEXT(addr);
        }

        // setAffinity(1);
        RDTSCP(ts0);
        addr = PNP_ADDR_NEXT(addr);
        RDTSCP(ts1);
        // setAffinity(0);

        if (ts1 - ts0 < THRESHOLD_BYTE)
            ret += 1.0 / rounds;
        // ret += (ts1 - ts0) / rounds;
    }
    return ret;
}

float __median(int n, uint64_t * x) {
    int temp;
    int i, j;
    // the following two loops sort the array x in ascending order
    for(i=0; i<n-1; i++) {
        for(j=i+1; j<n; j++) {
            if(x[j] < x[i]) {
                // swap elements
                temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }

    if(n%2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        return((x[n/2] + x[n/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        return x[n/2];
    }
}

void prefetch_profile_singlesetSimulation(struct mem_block * m, struct mem_block * victim_m, int page_idx, int rounds) {
    float result;
    int set_idx = 0;
    int probe_set = 0;
    int probe_line = 0;
    uint64_t set_mask = 0;

    FILE * fout = fopen(log_singleset, "w+");
    if (fout == 0)
        fout = fopen(log_singleset_android, "w+");
   
    int sets_to_test[] = {1};
    int _i;
    // for (_i = 0; _i < 2; _i ++) {
    //     set_idx = sets_to_test[_i];
    for (set_idx = 0; set_idx < N_LINES_PER_PAGE; set_idx ++) {
        // set_mask = set_mask | ((uint64_t)1 << set_idx);
        set_mask = ((uint64_t)1 << set_idx);
        fprintf(fout, "-------------------------------\n");
        fprintf(fout, "%llx\n", set_mask);
        for (probe_set = 0; probe_set < N_LINES_PER_PAGE; probe_set++) {
            fprintf(fout, "%d: ", probe_set);
            for (probe_line = 0; probe_line < (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); probe_line++) {
                result = 0.0;
                // for (page_idx = 0; page_idx < NUM_PAGES; page_idx ++) {
                result += prefetch_profile_traditionalMultiset(m, page_idx, set_mask, probe_set, probe_line, rounds);
                // }
                // result = result / NUM_PAGES;
                fprintf(fout, "%.02f ", result);
            }
            fprintf(fout, "\n", result);
            fflush(fout);
        }
    }

    fclose(fout);
}

void prefetch_profile_singlelineSimulation(struct mem_block * m, struct mem_block * victim_m, int page_idx, int rounds) {
    float result;
    int prime_set = 0;
    int prime_line = 0;
    int probe_set = 0;
    int probe_line = 0;
    uint64_t set_mask = 0;

    FILE * fout = fopen(log_singleline, "w+");

    for (prime_set = 0; prime_set < N_LINES_PER_PAGE; prime_set ++) {
        for (prime_line = 0; prime_line < (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); prime_line ++) {
            fprintf(fout, "-------------------------------\n");
            fprintf(fout, "%d %d\n", prime_set, prime_line);
            fprintf(stderr, "%d %d\n", prime_set, prime_line);
            for (probe_set = 0; probe_set < N_LINES_PER_PAGE; probe_set++) {
                fprintf(fout, "%d: ", probe_set);
                for (probe_line = 0; probe_line < (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); probe_line++) {
                    result = prefetch_profile_singleline(m, page_idx, prime_set, prime_line, probe_set, probe_line, rounds);
                    fprintf(fout, "%.02f ", result);
                }
                fprintf(fout, "\n", result);
                fflush(fout);
            }
        }
    }

    fclose(fout);
}

void prefetch_profile_main() {
    struct mem_block m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_SLOW | LINK_MODE_EXTRA_PRIME);
    struct mem_block victim_m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_SLOW | LINK_MODE_EXTRA_PRIME);

    // prefetch_profile_singlelineSimulation(&m, &victim_m, 0, 20);
    prefetch_profile_singlesetSimulation(&m, &victim_m, 0, 40);
    // multiset_simulation(&m, &victim_m, 0, 100);
    // traditional_pnp_simulation(&m, &victim_m, 0, 100);

    freeMem(&m);
    freeMem(&victim_m);
}

#endif
