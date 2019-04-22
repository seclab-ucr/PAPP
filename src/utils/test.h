#ifndef TEST_H
#define TEST_H

#include "global.h"
#include "mem_alloc.h"
#include "prime_probe.h"

static int ** test_pnp_ptr;
static char * test_pnp_result_ptr;
static int test_pnp_pages;
static int test_pnp_total;
static int test_pnp_round;
static int test_pnp_accesses[SHUFFLE_VECTOR_PRIME_LAST_NUM];

static void test_pnp_init(struct mem_block * m, char * result, int page_idx) {
    int i;
    // test_pnp_ptr = m->accessPointsByWays[page_idx * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS)];
    test_pnp_ptr = m->linkHead;
    test_pnp_result_ptr = result;
    if (m->linkMode & LINK_MODE_PRIME_LAST) {
        test_pnp_pages = m->numPages / N_OF_WAYS;
        for (int i = 0; i < SHUFFLE_VECTOR_PRIME_LAST_NUM; i ++) {
            test_pnp_accesses[i] = m->numProbesPerPage[i];
        }
    } else if (m->linkMode & LINK_MODE_EXTRA_PRIME) {
        test_pnp_pages = m->numPages / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
        test_pnp_accesses[0] = m->numProbesPerPage[0];
    }
    test_pnp_total = test_pnp_accesses[0];
    test_pnp_round = 0;
    // fprintf(stderr, "pnp_pages = %d\n", test_pnp_pages);
    // fprintf(stderr, "pnp_total = %d\n", test_pnp_total);
}

/*
 * Follow the default
 */
static void test_pnp_setLink(struct mem_block * m, int index, int measurement) {
    int i = index * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + measurement;
    int headIdx = index * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    int tailIdx = m->linkTailIndex % (N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS));
    // fprintf(stderr, "%d\t%#10x\n", tailIdx, m->accessPointsByWays[tailIdx]);
    // *(m->accessPointsByWays[m->linkTailIndex]) = m->accessPointsByWays[i];
    *(m->accessPointsByWays[tailIdx]) = m->accessPointsByWays[i];
}

static int ** test_pnp_setLinkSingleSet(struct mem_block * m, int page_idx, int cache_idx, int * accesses, int num_of_accesses, int measurement) {
    int i, j, k;
    int ** linkHead, **prev, **cur;
    int index = (page_idx * N_LINES_PER_PAGE + cache_idx) * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    prev = 0;
    // Flush
    for (i = 0; i < LINK_MODE_EXTRA_PRIME_WAYS; i++) {
        cur = m->accessPointsByWays[index + i];
        if (prev == 0) {
            prev = cur;
            linkHead = prev;
        } else {
            *prev = cur;
            prev = *prev;
        }
    }
    // Accesses
    for (i = 0; i < num_of_accesses; i++) {
        cur = m->accessPointsByWays[index + LINK_MODE_EXTRA_PRIME_WAYS + accesses[i]];
        if (prev == 0) {
            prev = cur;
            linkHead = prev;
        } else {
            *prev = cur;
            prev = *prev;
        }
    }
    // Measurement
    index = page_idx * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    cur = m->accessPointsByWays[index + measurement] + 1;
    *prev = cur;
    prev = *prev;
    *prev = -1;
    return linkHead;
}

static int ** test_pnp_setLinkPrefetchMap(struct mem_block * m, int page_idx, int cache_idx, int * accesses, int num_of_accesses, int measurement) {
    if (m->linkMode & LINK_MODE_EXTRA_PRIME)
        return -1;
    int i, j, k;
    int ** linkHead, **prev, **cur;
    int index = page_idx * N_LINES_PER_PAGE * N_OF_WAYS;
    prev = 0;
    // Accesses
    for (i = 0; i < num_of_accesses; i++) {
        cur = m->accessPointsByWays[index + cache_idx * N_OF_WAYS + accesses[i]];
        if (prev == 0) {
            prev = cur;
            linkHead = prev;
        } else {
            *prev = cur;
            prev = *prev;
        }
    }
    // Measurement
    index = page_idx * N_LINES_PER_PAGE * N_OF_WAYS;
    cur = m->accessPointsByWays[index + measurement] + 1;
    *prev = cur;
    prev = *prev;
    *prev = -1;
    return linkHead;
}

#define TEST_PROBE_CUSTOM(HEAD, I, TS0, TS1, ACCESSES) \
    test_pnp_ptr = HEAD; \
    for (I = ACCESSES; I > 0; I--) { \
        RDTSCP(TS0); \
        test_pnp_ptr = *test_pnp_ptr; \
        RDTSCP(TS1); \
        if (I == 1) { \
            *test_pnp_result_ptr = (TS1 - TS0) > THRESHOLD_BYTE; \
            test_pnp_result_ptr = test_pnp_result_ptr + 1; \
        } \
    }

#ifdef TEST_PNP_VICTIM
#define TEST_PROBE(I, J, TS0, TS1, VICTIM, ACCESSES) \
    for (J = ACCESSES; J >= 0; J--) { \
        if (J == 0) memset(VICTIM, -1, 4096); \
        RDTSCP(TS0); \
        test_pnp_ptr = *test_pnp_ptr; \
        RDTSCP(TS1); \
        if (J == 0) { \
            *test_pnp_result_ptr = (TS1 - TS0) > THRESHOLD_BYTE; \
            test_pnp_result_ptr = test_pnp_result_ptr + 1; \
        } \
    }
#else
#define TEST_PROBE(I, J, TS0, TS1, VICTIM, ACCESSES) \
    for (J = ACCESSES; J >= 0; J--) { \
        RDTSCP(TS0); \
        test_pnp_ptr = *test_pnp_ptr; \
        RDTSCP(TS1); \
        if (J == 0) { \
            *test_pnp_result_ptr = (TS1 - TS0) > THRESHOLD_BYTE; \
            test_pnp_result_ptr = test_pnp_result_ptr + 1; \
        } \
    }
#endif

#define TEST_PNP_ROUNDS 20
// Testing default prime & probe pattern
static void test_pnp_default() {
    struct mem_block m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_FAST | LINK_MODE_EXTRA_PRIME);
    struct mem_block victim_mem = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_LINEAR | LINK_MODE_SLOW);
    char * timestamps = malloc(N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(char));
    float * occupancy = malloc(N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(float));

    int i, j, k;
    int page_idx;
    int victim_idx;
    volatile register int test_pnp_i, test_pnp_j;
    volatile register unsigned int ts0, ts1;

    for (page_idx = 0; page_idx < 1; page_idx ++) {
        victim_idx = page_idx * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
        memset(timestamps, 0 , N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(char));
        for (i = 0; i < N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); i++) {
            occupancy[i] = 0.0;
        }

        printf("%d:\n", page_idx);
        for (k = 0; k < TEST_PNP_ROUNDS; k ++) {
            for (i = 0; i < N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); i++) {
                test_pnp_init(&m, timestamps + i, page_idx);
                test_pnp_setLink(&m, 0, i);
                flushMem(&m);
                TEST_PROBE(test_pnp_i, test_pnp_j, ts0, ts1, victim_mem.accessPointsByWays[victim_idx], test_pnp_total);
            }
            for (i = 0; i < N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); i++) {
                occupancy[i] += timestamps[i] * (1.0 / TEST_PNP_ROUNDS);
            }
        }
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            for (j = 0; j < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; j++) {
                printf("%1.2f ", occupancy[i * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + j]);
            }
            printf("\n");
        }
    }

    freeMem(&m);
    freeMem(&victim_mem);
    free(timestamps);
    free(occupancy);
}

static void test_pnp_single_set() {
    struct mem_block m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_FAST | LINK_MODE_EXTRA_PRIME);
    char * timestamps = malloc(N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(char));
    float * occupancy = malloc(N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(float));

    int i, j, k;
    int page_idx;
    int cache_idx;
    int n_accesses;
    int ** linkHead;
    volatile register int test_pnp_i, test_pnp_j;
    volatile register unsigned int ts0, ts1;
    int accesses[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    for (page_idx = 0; page_idx < 1; page_idx ++) {
      for (cache_idx = 10; cache_idx < 11; cache_idx ++) {
       for (n_accesses = 0; n_accesses <= N_OF_WAYS; n_accesses ++) {
        memset(timestamps, 0 , N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(char));
        for (i = 0; i < N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); i++) {
            occupancy[i] = 0.0;
        }

        printf("%d-%d-%d:\n", page_idx, cache_idx, n_accesses);
        for (k = 0; k < TEST_PNP_ROUNDS; k ++) {
            for (i = 0; i < N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); i++) {
                test_pnp_init(&m, timestamps + i, page_idx);
                linkHead = test_pnp_setLinkSingleSet(&m, page_idx, cache_idx, accesses, n_accesses, i);
                flushMem(&m);
                TEST_PROBE_CUSTOM(linkHead, test_pnp_i, ts0, ts1, LINK_MODE_EXTRA_PRIME_WAYS + n_accesses + 1);
            }
            for (i = 0; i < N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); i++) {
                occupancy[i] += timestamps[i] * (1.0 / TEST_PNP_ROUNDS);
            }
        }
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            for (j = 0; j < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; j++) {
                printf("%1.2f ", occupancy[i * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + j]);
            }
            printf("\n");
        }
       }
      }
    }

    freeMem(&m);
    free(timestamps);
    free(occupancy);
}

static void test_pnp_prefetch_map() {
    struct mem_block m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_FAST);
    char * timestamps = malloc(N_LINES_PER_PAGE * N_OF_WAYS * sizeof(char));
    float * occupancy = malloc(N_LINES_PER_PAGE * N_OF_WAYS * sizeof(float));

    int i, j, k;
    int page_idx;
    int cache_idx;
    int line_idx;
    int ** linkHead;
    volatile register int test_pnp_i, test_pnp_j;
    volatile register unsigned int ts0, ts1;

    for (page_idx = 0; page_idx < 1; page_idx ++) {
     for (cache_idx = 0; cache_idx < N_LINES_PER_PAGE; cache_idx ++) {
      for (line_idx = 0; line_idx < N_OF_WAYS; line_idx ++) {
        memset(timestamps, 0 , N_LINES_PER_PAGE * N_OF_WAYS * sizeof(char));
        for (i = 0; i < N_LINES_PER_PAGE * N_OF_WAYS; i++) {
            occupancy[i] = 0.0;
        }

        printf("%d-%d-%d:\n", page_idx, cache_idx, line_idx);
        for (k = 0; k < TEST_PNP_ROUNDS; k ++) {
            for (i = 0; i < N_LINES_PER_PAGE * N_OF_WAYS; i++) {
                test_pnp_init(&m, timestamps + i, page_idx);
                linkHead = test_pnp_setLinkPrefetchMap(&m, page_idx, cache_idx, &line_idx, 1, i);
                flushMem(&m);
                TEST_PROBE_CUSTOM(linkHead, test_pnp_i, ts0, ts1, 2);
            }
            for (i = 0; i < N_LINES_PER_PAGE * N_OF_WAYS; i++) {
                occupancy[i] += timestamps[i] * (1.0 / TEST_PNP_ROUNDS);
            }
        }
        for (i = 0; i < N_LINES_PER_PAGE; i++) {
            for (j = 0; j < N_OF_WAYS; j++) {
                printf("%1.2f ", occupancy[i * N_OF_WAYS + j]);
            }
            printf("\n");
        }
      }
     }
    }

    freeMem(&m);
    free(timestamps);
    free(occupancy);
}

static void testTsc() {
    unsigned long ts0, ts1;
    RDTSCP(ts0);
    RDTSCP(ts1);
    printf("%lu - %lu = %lu\n", ts1, ts0, ts1 - ts0);
}

static void test_pnp_main() {
    test_pnp_single_set();
    // test_pnp_prefetch_map();
    // test_pnp_default();
}

#endif
