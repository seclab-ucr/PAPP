#ifndef _SVF_TEST_H_
#define _SVF_TEST_H_

#include "../global.h"
#include "../utils/utils.h"
#include "../mem_alloc.h"

static int sets_to_test = N_LINES_PER_PAGE;

#include "svf_test_new.h"
#include "svf_test_traditional.h"

void svf_test_main() {
    struct mem_block m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_LINEAR | LINK_MODE_FAST | LINK_MODE_EXTRA_PRIME);
    struct mem_block victim_m = allocateMem(CACHE_SIZE, LINK_MODE_BYWAYS | LINK_MODE_LINEAR | LINK_MODE_FAST | LINK_MODE_EXTRA_PRIME);

    int i,j;
    int rounds = 1;
    int rounds_per_page = 50;
    uint64_t * tests = malloc((1+N_LINES_PER_PAGE) * sizeof(uint64_t));
    int num_tests;

    memset(tests, 0, (1+N_LINES_PER_PAGE) * sizeof(uint64_t));
    // Single set
    /*
    for (i = 0; i < N_LINES_PER_PAGE; i++) {
        tests[i+1] = ((uint64_t)1 << i);
    }
    num_tests = N_LINES_PER_PAGE + 1;

    // Consecutive 6 sets

    for (i = 0; i + 5 < N_LINES_PER_PAGE; i++) {
        tests[i+1] = 0;
        for (j = 0; j < 6; j++)
            tests[i+1] = tests[i+1] | ((uint64_t)1 << (i+j));
    }
    num_tests = N_LINES_PER_PAGE - 4;
    */

    // Random 6 sets
    int r = 0;
    int num_total_tests = 6;
    for (i = 1; i < N_LINES_PER_PAGE + 1; i++) {
        tests[i] = 0;
        j = 0;
        while (1) {
            r = rand() % N_LINES_PER_PAGE;
            if (((1 << r) & tests[i]) == 0) {
                tests[i] = tests[i] | ((uint64_t)1 << r);
                j ++;
            }
            if (j >= num_total_tests)
                break;
        }
    }
    num_tests = N_LINES_PER_PAGE + 1;

    svf_test_traditional(&m, &victim_m, tests, num_tests, rounds, rounds_per_page);
    svf_test_new(&m, &victim_m, tests, num_tests, rounds, rounds_per_page);

    freeMem(&m);
    freeMem(&victim_m);
}

#endif
