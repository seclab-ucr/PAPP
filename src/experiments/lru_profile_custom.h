#ifndef _LRU_PROFILE_CUSTOM_H_
#define _LRU_PROFILE_CUSTOM_H_

#include <sys/mman.h>

#include "lru_profile.h"
#include "svf_test.h"

static char lru_log_custom[64] = "lru_custom.log";

static char lru_log_custom_android[64] = "/mnt/sdcard/lru_custom.log";

float lru_profile_custom(struct mem_block * m, struct mem_block * victim_m, int page_idx, mem_access_point * prime_seq, int prime_seq_len, int probe_set, int probe_line, int rounds) {
    // Set link according to order for each page
    int i, j;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    PNP_ADDR bgn = 0, prev = 0;
    int prime_point;

    uint8_t block_used[64][32] = {};
    memset(block_used, 0, 64 * 32);

    // Prime sequence
    for (i = 0; i < prime_seq_len; i++) {
        prime_point = (page_idx * n_points_per_page) + (prime_seq[i].set_idx * n_points_per_set) + prime_seq[i].line_idx;
        if (bgn == 0) {
            bgn = m->accessPointsByWays[prime_point] + block_used[prime_seq[i].set_idx][prime_seq[i].line_idx] * sizeof(PNP_ADDR);
            prev = bgn;
        } else {
            PNP_ADDR_NEXT(prev) =  m->accessPointsByWays[prime_point] + block_used[prime_seq[i].set_idx][prime_seq[i].line_idx] * sizeof(PNP_ADDR);;
            prev = PNP_ADDR_NEXT(prev);
        }
        block_used[prime_seq[i].set_idx][prime_seq[i].line_idx] ++;
        // fprintf(stderr, "%#llx\n", prev);
    }

    // Probe sequence
    int probe_point = (page_idx * n_points_per_page) + (probe_set * n_points_per_set) + probe_line;
    PNP_ADDR probe_addr = m->accessPointsByWays[probe_point] + block_used[probe_set][probe_line] * sizeof(PNP_ADDR);
    // fprintf(stderr, "#%#llx#\n", m->accessPointsByWays[probe_point]);
    PNP_ADDR_NEXT(prev) = m->accessPointsByWays[probe_point] + block_used[probe_set][probe_line] * sizeof(PNP_ADDR);
    block_used[probe_set][probe_line]++;
    prev = PNP_ADDR_NEXT(prev);
    m->linkHead = bgn;

    // Test
    uint64_t ts0, ts1;
    int r;
    float ret_novic = 0.0;
    float ret_vic = 0.0;
    PNP_ADDR addr;
    PNP_ADDR victim_addr = victim_m->accessPointsByWays[page_idx * n_points_per_page];
    PNP_ADDR victim_addrs[64];

    PNP_ADDR * prime_addrs = malloc(prime_seq_len * sizeof(PNP_ADDR));
    for (i = 0; i < prime_seq_len; i++) {
        prime_point = (page_idx * n_points_per_page) + (prime_seq[i].set_idx * n_points_per_set) + prime_seq[i].line_idx;
        prime_addrs[i] = m->accessPointsByWays[prime_point];
    } 

    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        victim_addrs[i] = victim_m->accessPointsByWays[page_idx * n_points_per_page + i * n_points_per_set];
    }

    // No victim
    for (r = 0; r < rounds; r++) {
        addr = m->linkHead;
        // addr = probe_addr;
        _lru_clear_eviction_set(m, page_idx);
        MFENCE();

        for (i = 0; i < prime_seq_len; i++) {
            /*
            if (i % 32 >= 40) {
                LOAD(prime_addrs[i]);
                addr = prime_addrs[i+1];
            } else {
                addr = PNP_ADDR_NEXT(addr);
            }
            */
            addr = PNP_ADDR_NEXT(addr);
        }
        MFENCE();

        RDTSCP(ts0);
        // ts0 = rdtsc_bgn();
        // addr = PNP_ADDR_NEXT(addr);
        LOAD(probe_addr);
        RDTSCP(ts1);
        // ts1 = rdtsc_end();

        // ret_novic += (ts1 - ts0) / rounds;
        if (ts1 - ts0 < THRESHOLD_BYTE) ret_novic += 1.0 / rounds;
    }

    // With victim
    for (r = 0; r < rounds; r++) {
        addr = m->linkHead;
        addr = probe_addr;
        _lru_clear_eviction_set(m, page_idx);
        _lru_clear_eviction_set(victim_m, page_idx);
        MFENCE();

        for (i = 0; i < prime_seq_len; i++) {
            LOAD(prime_addrs[i]);
            // addr = PNP_ADDR_NEXT(addr);
        }

        MFENCE();
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            LOAD(victim_addrs[i]);
        }
        MFENCE();

        RDTSCP(ts0);
        // ts0 = rdtsc_bgn();
        // addr = PNP_ADDR_NEXT(addr);
        LOAD(probe_addr);
        RDTSCP(ts1);
        // ts1 = rdtsc_end();

        // ret_vic += (ts1 - ts0) / rounds;
        if (ts1 - ts0 < THRESHOLD_BYTE) ret_vic += 1.0 / rounds;
    }
    fprintf(stderr, "%d, %d:\t%.02f:%.02f\t%#llx (%#llx) - %#llx (%#llx)\n", probe_set, probe_line, ret_novic, ret_vic,
        probe_addr, getPhyAddr(probe_addr),
        victim_addrs[0], getPhyAddr(victim_addrs[0])
    );

    return ret_novic - ret_vic;
}

static int lru_profile_customPrimeSeq(struct mem_block * m, int page_idx, mem_access_point * prime_seq, int extra) {
    int i, j;
    int idx;
    for (i = 0; i < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; i++) {
        prime_seq[idx].set_idx = 0;
        prime_seq[idx].line_idx = i;
        idx++;
    }
    for (i = 0; i < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; i++) {
        if (i >= extra) {
            prime_seq[idx].set_idx = 1;
            prime_seq[idx].line_idx = i;
            idx++;
        }
    }
    return idx;
}

int lru_profile_defaultPrimeSeq(mem_access_point * seq) {
    int i, j, k;
    int cur, target;
    int idx = 0;
    int sets_primed = 1;
    for (i = 0; i < sets_primed; i++) {
        target = i % N_OF_WAYS;
        cur = 0;
        for (j = 0; j < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; j ++) {
            seq[idx].set_idx = i + 3;
            seq[idx].line_idx = j;
            idx ++;
        }
    }
    return idx;
}


void lru_profile_customSimulation(struct mem_block * m, struct mem_block * victim_m, int page_idx, mem_access_point * prime_seq, int prime_seq_len, int rounds) {
    float result;
    int set_idx = 0;
    int probe_set = 0;
    int probe_line = 0;
    uint64_t set_mask = 0;

    for (probe_set = 0; probe_set < 5; probe_set++) {
        for (probe_line = 0; probe_line < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; probe_line++) {
            result = 0.0;
            result += lru_profile_custom(m, victim_m, page_idx, prime_seq, prime_seq_len, probe_set, probe_line, rounds);
        }
    }
}

void lru_profile_custom_main() {
    int i;

    struct mem_block m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_SLOW | LINK_MODE_EXTRA_PRIME);
    struct mem_block victim_m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_SLOW | LINK_MODE_EXTRA_PRIME);

    mem_access_point * prime_seq = malloc(N_LINES_PER_PAGE * N_OF_WAYS * sizeof(mem_access_point));
    memset(prime_seq, 0, N_LINES_PER_PAGE * N_OF_WAYS * sizeof(mem_access_point));
    int prime_seq_len;

    int extra = 0;

    prime_seq_len = lru_profile_defaultPrimeSeq(prime_seq);
    fprintf(stderr, "Prime Sequence\n");
    for (i = 0; i < prime_seq_len; i++) {
        fprintf(stderr, "%d:%d ", prime_seq[i].set_idx, prime_seq[i].line_idx);
    }
    fprintf(stderr, "\n");

    lru_profile_customSimulation(&m, &victim_m, 0, prime_seq, prime_seq_len, 100);

    for (extra = 0; extra < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; extra++) {
        fprintf(stderr, "%d\n", extra);
        prime_seq_len = lru_profile_customPrimeSeq(&m, 0, prime_seq, extra);
        // prime_seq_len = lru_profile_defaultPrimeSeq(prime_seq);

        fprintf(stderr, "Prime Sequence\n");
        for (i = 0; i < prime_seq_len; i++) {
            fprintf(stderr, "%d:%d ", prime_seq[i].set_idx, prime_seq[i].line_idx);
        }
        fprintf(stderr, "\n");

        lru_profile_customSimulation(&m, &victim_m, 0, prime_seq, prime_seq_len, 100);
    }

    freeMem(&m);
    freeMem(&victim_m);
}

#endif
