#ifndef _SVF_TEST_TRADITIONAL_
#define _SVF_TEST_TRADITIONAL_

#define SVF_TEST_TRADITIONAL_PROBE_WAYS N_OF_WAYS
// #define SVF_TEST_TRADITIONAL_PRIME_WAYS N_OF_WAYS
#define SVF_TEST_TRADITIONAL_PRIME_WAYS (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS)

#define SVF_TEST_TRADITIONAL_LINK_MODE 2

static char svf_log_traditional[64] = "svf_traditional.log";
static char svf_log_traditional_android[64] = "/mnt/sdcard/svf_traditional.log";

void svf_test_setLinkTraditional(struct mem_block *m, int page_idx, int* sets_to_probe, int backward) {
    PNP_ADDR bgn = 0;
    PNP_ADDR prev = 0;

    int i, j;
    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    int point;

    // Round forward
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
    //     if (sets_to_probe[i] == 0)
    //         continue;
        if (sets_to_probe[i] < 0)
            break;
        for (j = 0; j < SVF_TEST_TRADITIONAL_PRIME_WAYS; j++) {
            // point = (page_idx * n_points_per_page) + (i * n_points_per_set) + j;
            point = (page_idx * n_points_per_page) + (sets_to_probe[i] * n_points_per_set) + j;
            if (bgn == 0) {
                bgn = m->accessPointsByWays[point];
                prev = bgn;
            } else {
                PNP_ADDR_NEXT(prev) =  m->accessPointsByWays[point];
                // fprintf(stderr, "%#llx -> %#llx\n", prev, PNP_ADDR_NEXT(prev));
                prev = PNP_ADDR_NEXT(prev);
            }
        }
    }

    // Round backward
    if (backward == 1) {
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            //     if (sets_to_probe[i] == 0)
            //         continue;
            if (sets_to_probe[i] < 0)
                break;
            for (j = SVF_TEST_TRADITIONAL_PRIME_WAYS - 1; j >= 0; j--) {
                point = (page_idx * n_points_per_page) + (sets_to_probe[i] * n_points_per_set) + j;
                PNP_ADDR_NEXT(prev) = m->accessPointsByWays[point] + sizeof(PNP_ADDR);
                // fprintf(stderr, "%#llx -> %#llx\n", prev, PNP_ADDR_NEXT(prev));
                prev = PNP_ADDR_NEXT(prev);
            }
        }
    } else if (backward == 2) {
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            // if (sets_to_probe[i] == 0)
            //     continue;
            if (sets_to_probe[i] < 0)
                break;
            for (j = SVF_TEST_TRADITIONAL_PROBE_WAYS; j < SVF_TEST_TRADITIONAL_PRIME_WAYS; j++) {
                point = (page_idx * n_points_per_page) + (sets_to_probe[i] * n_points_per_set) + j;
                PNP_ADDR_NEXT(prev) = m->accessPointsByWays[point] + sizeof(PNP_ADDR);
                prev = PNP_ADDR_NEXT(prev);
            }
            for (j = 0; j < SVF_TEST_TRADITIONAL_PROBE_WAYS; j++) {
                point = (page_idx * n_points_per_page) + (sets_to_probe[i] * n_points_per_set) + j;
                PNP_ADDR_NEXT(prev) = m->accessPointsByWays[point] + sizeof(PNP_ADDR);
                prev = PNP_ADDR_NEXT(prev);
            }
        }
    }

    PNP_ADDR_NEXT(prev) = bgn;
    m->linkHead = bgn;
}

// #define SVF_TEST_TRADITIONAL_ALT
#ifndef SVF_TEST_TRADITIONAL_ALT
void svf_test_pnpTraditional(struct mem_block *m, PNP_ADDR * addr, uint64_t * result, int num_sets) {
    // PNP_ADDR addr = m->linkHead;
    uint64_t ts0, ts1;
    int i, j;
    for (i = 0; i < num_sets; i++) {
        // fprintf(stderr, "%#llx\n", *addr);
        ts0 = readtsc();
        for (j = 0; j < SVF_TEST_TRADITIONAL_PROBE_WAYS; j++) {
            *addr = PNP_ADDR_NEXT(*addr);
        }
        ts1 = readtsc();
        for (; j < SVF_TEST_TRADITIONAL_PRIME_WAYS; j++) {
            *addr = PNP_ADDR_NEXT(*addr);
        }

        // Raw data
        result[i] = ts1 - ts0;
    }
}
#else
void svf_test_pnpTraditional(struct mem_block *m, PNP_ADDR * addr, uint64_t * result, int num_sets) {
    uint64_t ts0, ts1;
    int i, j;
    for (i = 0; i < num_sets; i++) {
        // fprintf(stderr, "%#llx\n", *addr);
        for (j = 0; j < SVF_TEST_TRADITIONAL_PROBE_WAYS - 1; j++) {
            *addr = PNP_ADDR_NEXT(*addr);
        }
        ts0 = readtsc();
        *addr = PNP_ADDR_NEXT(*addr);
        ts1 = readtsc();
        for (; j < SVF_TEST_TRADITIONAL_PRIME_WAYS; j++) {
            *addr = PNP_ADDR_NEXT(*addr);
        }

        // Raw data
        result[i] = ts1 - ts0;
    }
}
#endif

void svf_test_testSetTraditional(struct mem_block *m, struct mem_block *victim_m, int page_idx, int set_idx, int* sets_to_probe, int rounds, uint64_t * result) {
    // Count num of sets probed;
    int n_sets_probed;
    int i, j;
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        if (sets_to_probe[i] == 1)
            n_sets_probed ++;
    }
    uint64_t * result_tmp = malloc(n_sets_probed * sizeof(uint64_t));

    // Set Link
    svf_test_setLinkTraditional(m, page_idx, sets_to_probe, SVF_TEST_TRADITIONAL_LINK_MODE);

    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    int victim_access_point = (page_idx * n_points_per_page) + (set_idx * n_points_per_set);
    PNP_ADDR victim_addr;
    if (set_idx >= 0)
        victim_addr = victim_m->accessPointsByWays[victim_access_point];

    int r;
    PNP_ADDR addr;
    for (r = 0; r < rounds; r++) {
        if (set_idx >= 0) {
            addr = m->linkHead;
            // Prime & probe round 1
            svf_test_pnpTraditional(m, &addr, result_tmp, n_sets_probed);

            // Victim access
            MFENCE();
            LOAD(victim_addr);
            MFENCE();

            // Prime & probe round 2
            svf_test_pnpTraditional(m, &addr, result_tmp, n_sets_probed);
        } else {
            // Prime & probe round 1
            svf_test_pnpTraditional(m, &addr, result_tmp, n_sets_probed);
            // Prime & probe round 1
            svf_test_pnpTraditional(m, &addr, result_tmp, n_sets_probed);
        }

        j = 0;
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            if (sets_to_probe[i] == 1) {
                result[r * N_LINES_PER_PAGE + i] = result_tmp[j];
                j++;
            } else {
                result[r * N_LINES_PER_PAGE + i] = 0;
            }
            // fprintf(stderr, "%llu ", result[r * N_LINES_PER_PAGE + i]);
        }
        // fprintf(stderr, "\n");
    }

    free(result_tmp);
}

void svf_test_testSetTraditionalMultiset(struct mem_block *m, struct mem_block *victim_m, int page_idx, uint64_t set_mask, int* sets_to_probe, int rounds, uint64_t * result) {
    // Count num of sets probed;
    int n_sets_probed = 0;
    int i, j;
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        if (sets_to_probe[i] >= 0)
            n_sets_probed ++;
        else
            break;
    }
    uint64_t * result_tmp = malloc(n_sets_probed * sizeof(uint64_t));

    // Set Link
    svf_test_setLinkTraditional(m, page_idx, sets_to_probe, SVF_TEST_TRADITIONAL_LINK_MODE);

    int n_points_per_page = (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    int n_points_per_set = (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    // Victim
    int victim_access_point = (page_idx * n_points_per_page);
    PNP_ADDR victim_addr = victim_m->accessPointsByWays[victim_access_point];
    PNP_ADDR victim_addrs[64];
    int num_victim_addrs = 0;
    fprintf(stderr, "%#llx\n", set_mask);
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        if ((set_mask & ((uint64_t)1 << i)) > 0) {
            victim_access_point = (page_idx * n_points_per_page) + (i * n_points_per_set);
            victim_addrs[num_victim_addrs] = victim_m->accessPointsByWays[victim_access_point];
            // fprintf(stderr, "%#llx\n", victim_addrs[num_victim_addrs]);
            num_victim_addrs ++;
        }
    }

    int r;
    PNP_ADDR addr;
    for (r = 0; r < rounds; r++) {
        addr = m->linkHead;
        // Prime & probe round 1
        svf_test_pnpTraditional(m, &addr, result_tmp, n_sets_probed);

        // Victim access
        MFENCE();
        for (i = 0; i < num_victim_addrs; i++) {
            LOAD(victim_addrs[i]);
        }
        MFENCE();

        // Prime & probe round 2
        svf_test_pnpTraditional(m, &addr, result_tmp, n_sets_probed);

        for (i = 0; i < n_sets_probed; i++) {
            j = sets_to_probe[i];
            result[r * N_LINES_PER_PAGE + j] = result_tmp[i];
        }
    }

    free(result_tmp);
}



void svf_test_traditional(struct mem_block * m, struct mem_block * victim_m, uint64_t * tests, int num_tests, int rounds, int rounds_per_page) {
    int i, j, k, r;
    int set_idx, page_idx;

    int* sets_to_probe = malloc(N_LINES_PER_PAGE * sizeof(int));
    memset(sets_to_probe, 0, N_LINES_PER_PAGE * sizeof(int));
    // Every other set
    // for (i = 0; i < sets_to_test / 2; i++)
    //    sets_to_probe[i] = i * 2;
    // Every set
    for (i = 0; i < sets_to_test; i++)
        sets_to_probe[i] = i;
    sets_to_probe[sets_to_test / 2] = -1;
    printArray(sets_to_probe, sets_to_test / 2);
    randomizeArray(sets_to_probe, sets_to_test / 2);
    printArray(sets_to_probe, sets_to_test / 2);

    FILE * fout_traditional = fopen(svf_log_traditional, "w+");
    if (fout_traditional == 0)
        fout_traditional = fopen(svf_log_traditional_android, "w+");
    if (fout_traditional == 0) {
        fprintf(stderr, "Error. Cannot open log file.");
        exit(1);
    }

    uint64_t * result = malloc(rounds_per_page * N_LINES_PER_PAGE * sizeof(uint64_t));
    memset(result, 0, rounds_per_page * N_LINES_PER_PAGE * sizeof(uint64_t));

    for (i = 0; i < num_tests; i++) {
        fprintf(fout_traditional, "0x%llx\n", tests[i]);
        for (r = 0; r < rounds; r++) {
            page_idx = rounds % NUM_PAGES;
            svf_test_testSetTraditionalMultiset(m, victim_m, page_idx, tests[i], sets_to_probe, rounds_per_page, result);
            for (k = 0; k < rounds_per_page; k++) {
                for (j = 0; j < N_LINES_PER_PAGE; j++) {
                    fprintf(fout_traditional, "%llu ", result[k * N_LINES_PER_PAGE + j]);
                }
                fprintf(fout_traditional, "\n");
            }
        }
    }

    fclose(fout_traditional);
}

#endif
