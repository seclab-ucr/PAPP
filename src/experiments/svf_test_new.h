#ifndef _SVF_TEST_NEW_
#define _SVF_TEST_NEW_

#define SVF_TEST_NEW_EXTRA_WAY 16
#define SVF_TEST_DEFAULT_LRU (N_OF_WAYS) 

typedef struct mem_access_point_struct {
    int set_idx;
    int line_idx;
    // PNP_ADDR addr;
} mem_access_point;

static char svf_log_new[64] = "svf_new.log";
static char svf_log_new_android[64] = "/mnt/sdcard/svf_new.log";

#define SVF_TEST_NEW_SETS 64
int svf_test_defaultProbeSeq(mem_access_point * seq) {
    int i, j;
    int sets_probed = sets_to_test;
    for (i = 0; i < sets_probed; i++) {
        seq[i].set_idx = i;
        seq[i].line_idx = i % N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY;
    }
    return sets_probed;
}

int svf_test_defaultPrimeSeq(mem_access_point * seq) {
    int i, j;
    int cur, target;
    int idx = 0;
    int sets_primed = sets_to_test;
    for (i = 0; i < sets_primed; i++) {
        target = i % N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY;
        cur = 0;
        for (j = 0; j < N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY; j++) {
            if (j + SVF_TEST_DEFAULT_LRU == N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY) {
                seq[idx].set_idx = i;
                seq[idx].line_idx = target;
            } else {
                if (cur == target)
                    cur ++;
                seq[idx].set_idx = i;
                seq[idx].line_idx = cur;
                cur ++;
            }
            // fprintf(stderr, "%llx %d %d %d %d %d %d %d\n", &seq[idx], idx, i, j, cur, target,  seq[idx].set_idx,  seq[idx].line_idx);
            idx ++;
        }
    }
    return sets_primed * (N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY);
}

void svf_test_setLinkNew(struct mem_block *m, int page_idx, mem_access_point * probe_seq, int probe_seq_len, mem_access_point * prime_seq, int prime_seq_len) {
    PNP_ADDR bgn = 0;
    PNP_ADDR prev = 0;

    int i, j, point;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    // Probe sequence
    for (i = 0; i < probe_seq_len; i++) {  
        point = (page_idx * n_points_per_page) + (probe_seq[i].set_idx * n_points_per_set) + probe_seq[i].line_idx;
        if (bgn == 0) {
            bgn = m->accessPointsByWays[point];
            prev = bgn;
        } else {
            PNP_ADDR_NEXT(prev) =  m->accessPointsByWays[point];
            prev = PNP_ADDR_NEXT(prev);
        }
    }

    // Prime sequence
    for (i = 0; i < prime_seq_len; i++) { 
        point = (page_idx * n_points_per_page) + (prime_seq[i].set_idx * n_points_per_set) + prime_seq[i].line_idx;
        PNP_ADDR_NEXT(prev) =  m->accessPointsByWays[point] + sizeof(PNP_ADDR);
        prev = PNP_ADDR_NEXT(prev); 
    }

    PNP_ADDR_NEXT(prev) = bgn;
    m->linkHead = bgn;
}

void svf_test_pnpNew(struct mem_block *m, uint64_t * result, int probe_seq_len, int prime_seq_len) {
    PNP_ADDR addr = m->linkHead;
    uint64_t ts0, ts1;
    int i, j;
    // Probe
    for (i = 0; i < probe_seq_len; i++) {
        // fprintf(stderr, "%#llx\n", addr);
        ts0 = readtsc();
        addr = PNP_ADDR_NEXT(addr);
        ts1 = readtsc();
        result[i] = ts1 - ts0;
    }
    // Prime
    for (i = 0; i < prime_seq_len; i++) {
        // fprintf(stderr, "%#llx\n", addr);
        addr = PNP_ADDR_NEXT(addr);
    }
}

void svf_test_testSetNew(struct mem_block *m, struct mem_block *victim_m, uint64_t * result,
        int page_idx, int set_idx,
        mem_access_point * probe_seq, int probe_seq_len, mem_access_point * prime_seq, int prime_seq_len, int rounds) {
    int i;   
 
    // Set Link
    svf_test_setLinkNew(m, page_idx, probe_seq, probe_seq_len, prime_seq, prime_seq_len);

    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    // Victim
    int victim_access_point = (page_idx * n_points_per_page) + (set_idx * n_points_per_set);
    PNP_ADDR victim_addr;
    if (set_idx >= 0)
        victim_addr = victim_m->accessPointsByWays[victim_access_point];

    int r;
    uint64_t * result_tmp = malloc(N_LINES_PER_PAGE * sizeof(uint64_t));
    memset(result_tmp, 0, N_LINES_PER_PAGE * sizeof(uint64_t));
    for (r = 0; r < rounds; r++) {
        if (set_idx >= 0) {
            // Prime & probe round 1
            svf_test_pnpNew(m, result_tmp, probe_seq_len, prime_seq_len);

            // Victim access
            // MFENCE();
            LOAD(victim_addr);
            // MFENCE();

            // Prime & probe round 2
            svf_test_pnpNew(m, result_tmp, probe_seq_len, prime_seq_len);
        } else {
            // Prime & probe round 1
            svf_test_pnpNew(m, result_tmp, probe_seq_len, prime_seq_len);
            // Prime & probe round 2
            svf_test_pnpNew(m, result_tmp, probe_seq_len, prime_seq_len);
        }

        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            result[r * N_LINES_PER_PAGE + i] = result_tmp[i];
        }
    }
    free(result_tmp);
}

void svf_test_testSetNewMultiset(struct mem_block *m, struct mem_block *victim_m, uint64_t * result,
        int page_idx, uint64_t set_mask,
        mem_access_point * probe_seq, int probe_seq_len, mem_access_point * prime_seq, int prime_seq_len, int rounds) {
    int i;

    // Set Link
    svf_test_setLinkNew(m, page_idx, probe_seq, probe_seq_len, prime_seq, prime_seq_len);

    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    // Victim
    int victim_access_point = (page_idx * n_points_per_page);
    PNP_ADDR victim_addr = victim_m->accessPointsByWays[victim_access_point];
    PNP_ADDR victim_addrs[64];
    int num_victim_addrs = 0;
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        if ((set_mask & ((uint64_t)1 << i)) > 0) {
            victim_access_point = (page_idx * n_points_per_page) + (i * n_points_per_set);
            victim_addrs[num_victim_addrs] = victim_m->accessPointsByWays[victim_access_point];
            // fprintf(stderr, "%#llx\n", victim_addrs[num_victim_addrs]);
            num_victim_addrs ++;
        }
    }

    int r;
    uint64_t * result_tmp = malloc(N_LINES_PER_PAGE * sizeof(uint64_t));
    memset(result_tmp, 0, N_LINES_PER_PAGE * sizeof(uint64_t));
    for (r = 0; r < rounds; r++) {
        // Prime & probe round 1
        svf_test_pnpNew(m, result_tmp, probe_seq_len, prime_seq_len);

        // Victim access
        MFENCE();
        for (i = 0; i < num_victim_addrs; i++) {
            LOAD(victim_addrs[i]);
        }
        MFENCE();

        // Prime & probe round 2
        svf_test_pnpNew(m, result_tmp, probe_seq_len, prime_seq_len);

        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            result[r * N_LINES_PER_PAGE + i] = result_tmp[i];
        }
    }
    free(result_tmp);
}

void svf_test_new(struct mem_block *m, struct mem_block *victim_m, uint64_t * tests, int num_tests, int rounds, int rounds_per_page) {
    int i, j, k, r;
    int set_idx, page_idx;

    // Setup sequence
    mem_access_point * prime_seq = malloc(N_LINES_PER_PAGE * (N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY) * sizeof(mem_access_point));
    memset(prime_seq, 0, N_LINES_PER_PAGE * (N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY) * sizeof(mem_access_point));
    mem_access_point * probe_seq = malloc(N_LINES_PER_PAGE * sizeof(mem_access_point));
    memset(probe_seq, 0, N_LINES_PER_PAGE * sizeof(mem_access_point));
    int prime_seq_len = svf_test_defaultPrimeSeq(prime_seq);
    int probe_seq_len = svf_test_defaultProbeSeq(probe_seq);

    fprintf(stderr, "Probe Sequence\n");
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        fprintf(stderr, "%d:%d ", probe_seq[i].set_idx, probe_seq[i].line_idx);
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "Prime Sequence\n");
    for (i = 0; i < N_LINES_PER_PAGE * (N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY); i++) {
        fprintf(stderr, "%d:%d ", prime_seq[i].set_idx, prime_seq[i].line_idx);
        if ((i+1) % (N_OF_WAYS + SVF_TEST_NEW_EXTRA_WAY) == 0)
            fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    
    // Test
    FILE * fout_new = fopen(svf_log_new, "w+");
    if (fout_new == 0)
        fout_new = fopen(svf_log_new_android, "w+");
    if (fout_new == 0) {
        fprintf(stderr, "Error. Cannot open log file.");
        exit(1);
    }

    uint64_t * result = malloc(rounds_per_page * N_LINES_PER_PAGE * sizeof(uint64_t));
    memset(result, 0, rounds_per_page * N_LINES_PER_PAGE * sizeof(uint64_t));

    for (i = 0; i < num_tests; i++) {
        fprintf(fout_new, "0x%llx\n", tests[i]);
        for (r = 0; r < rounds; r++) {
            page_idx = rounds % NUM_PAGES;
            svf_test_testSetNewMultiset(m, victim_m, result, page_idx, tests[i], probe_seq, probe_seq_len, prime_seq, prime_seq_len, rounds_per_page);
            for (k = 0; k < rounds_per_page; k++) {
                for (j = 0; j < N_LINES_PER_PAGE; j++) {
                    fprintf(fout_new, "%llu ", result[k * N_LINES_PER_PAGE + j]);
                }
                fprintf(fout_new, "\n");
            }
        }
    }

    fclose(fout_new);
    // free(result);
}

#endif
