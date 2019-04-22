#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "utils/utils.h"
#include "global.h"

#define MAX_ALLOC_COEFFICIENT 64
#define NUM_PAGES_PER_BLOCK 32

#define LINK_MODE_BYSETS 0x1
#define LINK_MODE_BYWAYS 0x2
#define LINK_MODE_PRIME_LAST 0x4

#define LINK_MODE_SHUFFLE_LINES 0x8

#define LINK_MODE_LINEAR 0x10
#define LINK_MODE_RANDOM 0x20
#define LINK_MODE_PSEUDO_RANDOM 0x40
#define LINK_MODE_REVERSE 0x80
#define LINK_MODE_EXTRA_PRIME 0x100
#define LINK_MODE_4MB_EXTRA 0x200

// Requirement: LINK_MODE_EXTRA_PRIME_WAYS < N_OF_WAYS
#ifndef LINK_MODE_EXTRA_PRIME_WAYS
#define LINK_MODE_EXTRA_PRIME_WAYS N_OF_WAYS
#endif

#define LINK_MODE_SLOW 0x1000
#define LINK_MODE_FAST 0x2000

#define MEM_BLOCK_MMAP

/*
static int shuffle_vector[N_LINES_PER_PAGE] = {
        0, 10, 44, 4, 49, 21, 14, 26,
        12, 8, 19, 16, 28, 37, 42, 23,
        59, 53, 58, 46, 20, 52, 27, 24,
        1, 41, 13, 32, 43, 25, 55, 2,
        61, 29, 40, 60, 62, 18, 3, 11,
        51, 31, 33, 30, 63, 38, 47, 48,
        45, 6, 57, 5, 15, 56, 9, 54,
        35, 39, 22, 7, 36, 34, 17, 50
};
*/

static int shuffle_vector[128] = {
        0, 7, 14, 21, 28, 35, 42, 49,
        56, 62, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
};

#define SHUFFLE_VECTOR_PRIME_LAST_LEN 57
#define SHUFFLE_VECTOR_PRIME_LAST_NUM 2
#define SHUFFLE_VECTOR_PRIME_LAST_NUM_MAX 8

static int shuffle_vector_prime_last[SHUFFLE_VECTOR_PRIME_LAST_NUM_MAX][SHUFFLE_VECTOR_PRIME_LAST_LEN] = {
        {},
        {
            118,            128, 145, 162, 179, 196, 213, 230,            240, 257, 274, 291, 308, 325, 342,            352, 369, 386, 403, 420, 437, 454,            464, 481, 498, 515, 532, 549, 566,            576, 593, 610, 627, 644, 661, 678,            688, 705, 722, 739, 756, 773, 790,            800, 817, 834, 851, 868, 885, 902,            912, 929, 946, 963, 980, 997, 1014
        },
        {},
        {},
        {},
        {},
        {},
        {}
};

#define SHUFFLE_VECTOR_EXTRA_PRIME_LEN 63
/*
static int shuffle_vector_extra_prime[SHUFFLE_VECTOR_EXTRA_PRIME_LEN] = {
        112, 129, 146, 163, 180, 197, 214,        224, 241, 258, 275, 292, 309, 326,        336, 353, 370, 387, 404, 421, 438,        448, 465, 482, 499, 516, 533, 550,        560, 577, 594, 611, 628, 645, 662,        672, 689, 706, 723, 740, 757, 774,        784, 801, 818, 835, 852, 869, 886,        896, 913, 930, 947, 964, 981, 998,        1008
    };
    */

static int shuffle_vector_extra_prime_set[SHUFFLE_VECTOR_EXTRA_PRIME_LEN] = {
              1,  2,  3,  4,  5,  6,
          7,  8,  9, 10, 11, 12, 13,
         14, 15, 16, 17, 18, 19, 20,
         21, 22, 23, 24, 25, 26, 27,
         28, 29, 30, 31, 32, 33, 34,
         35, 36, 37, 38, 39, 40, 41,
         42, 43, 44, 45, 46, 47, 48,
         49, 50, 51, 52, 53, 54, 55,
         56, 57, 58, 59, 60, 61, 62,
         63
};
static int shuffle_vector_extra_prime_line[SHUFFLE_VECTOR_EXTRA_PRIME_LEN] = {
            1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0, 1, 2, 3, 4, 5, 6,
         0
};
/*
static int shuffle_vector_extra_prime_line[SHUFFLE_VECTOR_EXTRA_PRIME_LEN] = {
            0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,
         0
};
 */

/*
static int shuffle_vector_prime_last[SHUFFLE_VECTOR_PRIME_LAST_NUM_MAX][SHUFFLE_VECTOR_PRIME_LAST_LEN] = {
        {
            112, 129, 146, 163, 180, 197, 214,            224, 241, 258, 275, 292, 309, 326,            336, 353, 370, 387, 404, 421, 438,            448, 465, 482, 499, 516, 533, 550,            560, 577, 594, 611, 628, 645, 662,            672, 689, 706, 723, 740, 757, 774,            784, 801, 818, 835, 852, 869, 886,            896, 913, 930, 947, 964, 981, 998,            1008
        },
        {
            113, 130, 147, 164, 181, 198,            208, 225, 242, 259, 276, 293, 310,            320, 337, 354, 371, 388, 405, 422,            432, 449, 466, 483, 500, 517, 534,            544, 561, 578, 595, 612, 629, 646,            656, 673, 690, 707, 724, 741, 758,            768, 785, 802, 819, 836, 853, 870,            880, 897, 914, 931, 948, 965, 982,            992, 1009
        },
        {
            114, 131, 148, 165, 182,            192, 209, 226, 243, 260, 277, 294,            304, 321, 338, 355, 372, 389, 406,            416, 433, 450, 467, 484, 501, 518,            528, 545, 562, 579, 596, 613, 630,            640, 657, 674, 691, 708, 725, 742,            752, 769, 786, 803, 820, 837, 854,            864, 881, 898, 915, 932, 949, 966,            976, 993, 1010
        },
        {
            115, 132, 149, 166,            176, 193, 210, 227, 244, 261, 278,            288, 305, 322, 339, 356, 373, 390,            400, 417, 434, 451, 468, 485, 502,            512, 529, 546, 563, 580, 597, 614,            624, 641, 658, 675, 692, 709, 726,            736, 753, 770, 787, 804, 821, 838,            848, 865, 882, 899, 916, 933, 950,            960, 977, 994, 1011
        },
        {
            116, 133, 150,            160, 177, 194, 211, 228, 245, 262,            272, 289, 306, 323, 340, 357, 374,            384, 401, 418, 435, 452, 469, 486,            496, 513, 530, 547, 564, 581, 598,            608, 625, 642, 659, 676, 693, 710,            720, 737, 754, 771, 788, 805, 822,            832, 849, 866, 883, 900, 917, 934,            944, 961, 978, 995, 1012
        },
        {
            117, 134,             144, 161, 178, 195, 212, 229, 246,            256, 273, 290, 307, 324, 341, 358,            368, 385, 402, 419, 436, 453, 470,            480, 497, 514, 531, 548, 565, 582,            592, 609, 626, 643, 660, 677, 694,            704, 721, 738, 755, 772, 789, 806,            816, 833, 850, 867, 884, 901, 918,            928, 945, 962, 979, 996, 1013
        },
        {
            118,            128, 145, 162, 179, 196, 213, 230,            240, 257, 274, 291, 308, 325, 342,            352, 369, 386, 403, 420, 437, 454,            464, 481, 498, 515, 532, 549, 566,            576, 593, 610, 627, 644, 661, 678,            688, 705, 722, 739, 756, 773, 790,            800, 817, 834, 851, 868, 885, 902,            912, 929, 946, 963, 980, 997, 1014
        },
        {}
};
*/

/*
static int shuffle_vector_prime_last[SHUFFLE_VECTOR_PRIME_LAST_NUM][SHUFFLE_VECTOR_PRIME_LAST_LEN] = {
        {
                112, 129, 146, 163, 180, 197, 214, 224,
                241, 258, 275, 292, 309, 326, 336, 353,
                370, 387, 404, 421, 438, 448, 465, 482,
                499, 516, 533, 550, 560, 577, 594, 611,
                624, 644, 656, 677, 689, 706, 723, 742,
                756, 775, 784, 801, 818, 835, 853, 868,
                886, 903, 913, 930, 947, 960, 980, 997,
                1014
        },
        {
                112, 129, 146, 163, 180, 197, 214, 224,
                241, 258, 275, 292, 309, 326, 336, 353,
                370, 387, 404, 421, 438, 448, 465, 482,
                499, 516, 533, 550, 560, 577, 594, 611,
                624, 644, 656, 677, 689, 706, 723, 742,
                756, 775, 784, 801, 818, 835, 853, 868,
                886, 903, 913, 930, 947, 960, 980, 997,
                1014
        }
};
*/

static void printMemPages(struct mem_block * m) {
#ifndef ANDROID
    int i, paddr;
    printf("Num of Pages Used:\t%d\n", m->numPages);
    printf("Num of Pages Allocated:\t%d\n",
            m->numBlocksUsed * NUM_PAGES_PER_BLOCK);
    i = 0;
    uint64_t * paddrs = malloc(m->numPages * sizeof(uint64_t));
    getPhyAddrs(m->mapping, paddrs, m->numPages);
    while (i < m->numPages) {
        printf("%#10x -> %#10x (%#10x)\n", i, m->mapping[i], paddrs[i]);
        i = i + 1;
    }
    free(paddrs);
#endif
}

static void printMemAccessPoints(struct mem_block * m) {
#ifndef ANDROID
    int i;
    long * paddrs;
    printf("Num of Pages Used:\t%d\n", m->numPages);
    printf("Num of Pages Allocated:\t%d\n",
            m->numBlocksUsed * NUM_PAGES_PER_BLOCK);
    i = 0;
    if (m->linkMode & LINK_MODE_BYSETS) {
        paddrs = malloc(m->numCacheLines * sizeof(char *));
        getPhyAddrs(m->accessPoints, paddrs, m->numCacheLines);
        while (i < m->numCacheLines) {
            printf("%#018x -> %#018x (%#018x)\n", i, m->accessPoints[i],
                    paddrs[i]);
            i = i + 1;
        }
    } else {
        paddrs = malloc(m->numCacheLines * sizeof(char *));
        getPhyAddrs(m->accessPointsByWays, paddrs, m->numCacheLines);
        while (i < m->numCacheLines) {
            printf("%#10x [%#018x] -> %#018x (%#18x) [%#018x]\n", i,
                    m->shuffleMap[i], m->accessPointsByWays[i], paddrs[i],
                    *(uint64_t*)(m->accessPointsByWays[i]));
            i = i + 1;
        }
    }

    free(paddrs);
#endif
}

#ifdef MEM_ALLOC_DEFAULT_LINK
static void setLinkBySets(struct mem_block * m) {
    int i, j, k, n, round;
    int **prev_ptr;
    int prev, cur, count;
    int bgn;
    if (m->linkMode & LINK_MODE_FAST) {
        i = 0;
        prev = -1;
        bgn = -1;
        while (i < m->numCacheLines - 1) {
            if (i % LINK_MODE_BIN_STEP < LINK_MODE_BIN_SIZE) {
                if (prev >= 0)
                    *(m->accessPoints[prev]) = m->accessPoints[i];
                if (bgn < 0)
                    bgn = i;
                prev = i;
            }
            i = i + 1;
        }
        m->linkHead = m->accessPointsByWays[bgn];
        if (prev >= 0)
            *(m->accessPointsByWays[prev]) = m->linkHead;
    } else { // Default: (m->linkMode & LINK_MODE_SLOW)
        i = 0;
        while (i < (m->numCacheLines / 4) - 1) {
            *(m->accessPoints[i]) = m->accessPoints[i + 1];
            i = i + 1;
        }
        *(m->accessPoints[i]) = 0;
        m->linkHead = m->accessPointsByWays[0];
    }
}

static void setLinkByWays(struct mem_block * m) {
    int i, j, k, n, round;
    int **prev_ptr;
    int prev, cur, count;
    int bgn;
    if (m->linkMode & LINK_MODE_FAST) {
        i = 0;
        prev = -1;
        bgn = -1;
        while (i < m->numCacheLines) {
            // FPRINTF_ERR"%d %d %d\n", prev, i, (i / N_OF_WAYS) % LINK_MODE_BIN_STEP);
            if ((i / N_OF_WAYS) % LINK_MODE_BIN_STEP < LINK_MODE_BIN_SIZE) {
                if (prev >= 0) {
                    // FPRINTF_ERR"%#10x %#10x\n", m->accessPointsByWays[prev], m->accessPointsByWays[i]);
                    *(m->accessPointsByWays[prev]) = m->accessPointsByWays[i];
                }
                if (bgn < 0)
                    bgn = i;
                prev = i;
            }
            i = i + 1;
        }
        m->linkHead = m->accessPointsByWays[bgn];
        if (prev >= 0) {
            *(m->accessPointsByWays[prev]) = m->linkHead;
            m->linkTailIndex = prev;
        }
    } else { // Default: (m->linkMode & LINK_MODE_SLOW
        for (i = 0; i < m->numCacheLines - 1; i++) {
            *(m->accessPointsByWays[i]) = m->accessPointsByWays[i + 1];
        }
        *(m->accessPointsByWays[i]) = 0;
        m->linkTailIndex = i;
        m->linkHead = m->accessPointsByWays[0];
    }
}

static void setLinkPrimeLast(struct mem_block * m) {
    int i, j, k, n, round;
    int **prev_ptr;
    int prev, cur, count;
    int bgn;
    char * used_lines;
    char * used_sets;
    bgn = 0;
    prev_ptr = 0;
    used_lines = malloc(m->numCacheLines * sizeof(char));
    used_sets = malloc(m->numCacheLines / N_OF_WAYS * sizeof(char));
    m->numProbesPerPage = malloc(SHUFFLE_VECTOR_PRIME_LAST_NUM * sizeof(int));

    for (round = 0; round < SHUFFLE_VECTOR_PRIME_LAST_NUM; round++) {
        memset(used_lines, 0, m->numCacheLines * sizeof(char));
        memset(used_sets, 0, m->numCacheLines / N_OF_WAYS * sizeof(char));
        m->numProbesPerPage[round] = 0;
        for (i = 0; i < m->numPages / N_OF_WAYS; i++) {
            count = 0;
            // TLB prime
            // FPRINTF_ERR"TLB Prime %d\n", prev);
            for (j = 0; j < N_OF_WAYS; j++) {
                cur = i * N_LINES_PER_PAGE * N_OF_WAYS + j;
                used_lines[cur] = 1;
                used_sets[cur / N_OF_WAYS] = 1;
                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur] + round;
                }
                count ++;
                prev_ptr = m->accessPointsByWays[cur] + round;
            }
            // Probe
            // FPRINTF_ERR"Probe %d\n", prev);
            n = SHUFFLE_VECTOR_PRIME_LAST_LEN;
            if (m->linkMode & LINK_MODE_FAST) {
                n = LINK_MODE_BIN_SIZE;
            }
            for (j = 0; j < n; j++) {
                cur = i * N_LINES_PER_PAGE * N_OF_WAYS
                        + shuffle_vector_prime_last[round][j];
                used_lines[cur] = 1;
                // Prefetching issue???

                for (k = -6; k <= 6; k++) {
                    if (cur + k * N_OF_WAYS >= 0 && cur + k * N_OF_WAYS < m->numCacheLines)
                        used_lines[cur + k * N_OF_WAYS] = 1;
                }

                used_sets[cur / N_OF_WAYS] = 1;
                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur] + round;
                }
                prev_ptr = m->accessPointsByWays[cur] + round;
                count ++;
            }
            // Prime
            // FPRINTF_ERR"Prime %d\n", prev);
            for (j = 0; j < N_LINES_PER_PAGE * N_OF_WAYS; j++) {
                cur = i * N_LINES_PER_PAGE * N_OF_WAYS + j;
                if (used_sets[cur / N_OF_WAYS] != 1) {
                    continue;
                }
                if (used_lines[cur] == 0) {
                    if (prev_ptr != 0) {
                        *(prev_ptr) = m->accessPointsByWays[cur] + round;
                    }
                    used_lines[cur] = 1;

                    /*
                    for (k = 1; k <= 6; k++) {
                        if (cur + k * N_OF_WAYS >= 0 && cur + k * N_OF_WAYS < m->numCacheLines)
                            used_lines[cur + k * N_OF_WAYS] = 1;
                    }
                    */

                    prev_ptr = m->accessPointsByWays[cur] + round;
                    count ++;
                }
            }
            if (m->numProbesPerPage[round] == 0) {
                m->numProbesPerPage[round] = count;
            }
        }
    }
    m->linkHead = m->accessPointsByWays[0];
    if (prev_ptr != 0)
        *(prev_ptr) = m->linkHead;
    free(used_lines);
    free(used_sets);
}

static void setLinkExtraPrime(struct mem_block * m) {
    int i, j, k, l, n, round;
    int **prev_ptr;
    int set, line;
    int prev, cur, count;
    int bgn;
    char * used_sets;
    char * used_lines;
    used_lines = malloc(m->numCacheLines * sizeof(char));
    memset(used_lines, 0, m->numCacheLines * sizeof(char));
    used_sets = malloc(m->numCacheLines / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(char));
    memset(used_sets, 0, m->numCacheLines / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * sizeof(char));
    m->numProbesPerPage = malloc(sizeof(int));
    bgn = 0;
    prev_ptr = 0;

    for (i = 0; i < m->numPages / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS); i++) {
        // TLB Prime
        for (j = 0; j < N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS; j++) {
            cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + j;
            if (prev_ptr != 0) {
                *(prev_ptr) = m->accessPointsByWays[cur];
            }
            count++;
            prev_ptr = m->accessPointsByWays[cur];
        }

        // Probe
        n = SHUFFLE_VECTOR_EXTRA_PRIME_LEN;
        if (m->linkMode & LINK_MODE_FAST) {
            n = LINK_MODE_BIN_SIZE;
        }
        for (j = 0; j < n; j++) {
            cur = (i * N_LINES_PER_PAGE + shuffle_vector_extra_prime_set[j]) * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + shuffle_vector_extra_prime_line[j] + LINK_MODE_EXTRA_PRIME_WAYS;
            used_sets[cur / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS)] = 1;
            /*
            if (prev_ptr != 0) {
                *(prev_ptr) = m->accessPointsByWays[cur] + 1;
            }
            prev_ptr = *(prev_ptr);
            count++;
            */
        }

        // Prime
        for (j = 0; j < N_LINES_PER_PAGE; j++) {
            if (used_sets[i * N_LINES_PER_PAGE + j] == 0) {
                continue;
            }
            // Prime First Half
            for (k = 0; k < LINK_MODE_EXTRA_PRIME_WAYS; k++) {
                cur = (i * N_LINES_PER_PAGE + j) * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + k;

                // Skipping
                if (used_lines[cur] == 1)
                    continue;
                used_lines[cur] = 1;
                for (l = 1; l <= 0; l ++) {
                    if (cur + ((N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * l) >= 0 && cur + ((N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * l) < m->numCacheLines)
                        used_lines[cur + ((N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * l)] = 1;
                }

                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur];
                }
                prev_ptr = *(prev_ptr);
                count++;
            }
            // First line in a set
            cur = (i * N_LINES_PER_PAGE + j) * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + (j % 7) + LINK_MODE_EXTRA_PRIME_WAYS;
            if (prev_ptr != 0) {
                *(prev_ptr) = m->accessPointsByWays[cur];
            }
            prev_ptr = *(prev_ptr);
            count++;
            // Rest lines
            for (k = 0; k < N_OF_WAYS; k++) {
                if (k == (j % 7))
                    continue;
                cur = (i * N_LINES_PER_PAGE + j) * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + k + LINK_MODE_EXTRA_PRIME_WAYS;

                // Skipping
                if (used_lines[cur] == 1)
                    continue;
                used_lines[cur] = 1;
                for (l = 1; l <= 0; l ++) {
                    if (cur + ((N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * l) >= 0 && cur + ((N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * l) < m->numCacheLines)
                        used_lines[cur + ((N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) * l)] = 1;
                }

                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur];
                }
                prev_ptr = *(prev_ptr);
                count++;
            }
        }
        if (m->numProbesPerPage[0] == 0) {
            m->numProbesPerPage[0] = count;
        }
    }
    m->linkHead = m->accessPointsByWays[0];
    if (prev_ptr != 0) {
        *(prev_ptr) = m->linkHead;
        m->linkTailIndex = cur;
    }
    free(used_lines);
    free(used_sets);
}
#endif

static int __setLinkCustom(struct mem_block * m,
        int * boot_seq, int boot_seq_len,
        int * probe_seq, int probe_seq_len,
        int * prime_seq, int prime_seq_len,
        int page_mask) {
    int i, j, k, l, n, round;
    uint64_t prev_ptr;
    int set, line;
    int prev, cur, count = 0;
    int bgn;
    m->numProbesPerPage = malloc(sizeof(int));
    bgn = 0;
    prev_ptr = 0;
    m->linkHead = 0;
    int max_pages = m->numPages / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    for (i = 0; i < max_pages; i++) {
        if ((page_mask & (1 << i)) == 0)
            continue;
        // Boot Sequence
        if (boot_seq != 0 && boot_seq_len != 0) {
            for (j = 0; j < boot_seq_len; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + boot_seq[j];
                if (prev_ptr != 0) {
                    PNP_ADDR_NEXT(prev_ptr) = m->accessPointsByWays[cur] + 0;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + 0;
                }
                prev_ptr = m->accessPointsByWays[cur] + 0;
                count++;
            }
        }
        // Probe Sequence
        if (probe_seq != 0 && probe_seq_len != 0) {
            for (j = 0; j < probe_seq_len; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + probe_seq[j];
                if (prev_ptr != 0) {
                    PNP_ADDR_NEXT(prev_ptr) = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 1;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 1;
                }
                prev_ptr = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 1;
                count++;
            }
        }
        // Prime Sequence
        if (prime_seq != 0 && prime_seq_len != 0) {
            for (j = 0; j < prime_seq_len; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + prime_seq[j];
                if (prev_ptr != 0) {
                    PNP_ADDR_NEXT(prev_ptr) = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 2;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 2;
                }
                prev_ptr = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 2;
                count++;
            }
        }
        if (m->numProbesPerPage[0] == 0) {
            m->numProbesPerPage[0] = count;
        }
    }
    if (prev_ptr != 0)
        *((uint64_t*)prev_ptr) = m->linkHead;
    return count;
}

#ifdef TRADITIONAL_PNP
static int __setLinkAltSeq(struct mem_block * m,
        int * seq0, int seq0_len,
        int * seq1, int seq1_len,
        int page_mask) {
    int i, j, k, l, n, round;
    int **prev_ptr;
    int set, line;
    int prev, cur, count = 0;
    int bgn;
    m->numProbesPerPage = malloc(sizeof(int));
    bgn = 0;
    prev_ptr = 0;
    m->linkHead = 0;
    int max_pages = m->numPages / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    for (i = 0; i < max_pages; i++) {
        if ((page_mask & (1 << i)) == 0)
            continue;
        if (seq0 != 0 && seq0_len != 0) {
            for (j = 0; j < seq0_len; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + seq0[j];
                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur] + 0;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + 0;
                }
                prev_ptr = m->accessPointsByWays[cur] + 0;
                count++;
            }
        }
        if (m->numProbesPerPage[0] == 0) {
            m->numProbesPerPage[0] = count;
        }
    }

    for (i = 0; i < max_pages; i++) {
        if ((page_mask & (1 << i)) == 0)
            continue;
        if (seq1 != 0 && seq1_len != 0) {
            for (j = 0; j < seq1_len; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + seq1[j];
                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur] + 1;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + 1;
                }
                prev_ptr = m->accessPointsByWays[cur] + 1;
                count++;
            }
        }
        if (m->numProbesPerPage[0] == 0) {
            m->numProbesPerPage[0] = count;
        }
    }

    if (prev_ptr != 0)
        *(prev_ptr) = m->linkHead;
    return count;
}

static int __setLinkAltSeqPerPage(struct mem_block * m,
        int ** seq0, int * seq0_len,
        int ** seq1, int * seq1_len,
        int page_mask) {
    int i, j, k, l, n, round;
    int **prev_ptr;
    int set, line;
    int prev, cur, count = 0;
    int bgn;
    m->numProbesPerPage = malloc(sizeof(int));
    bgn = 0;
    prev_ptr = 0;
    m->linkHead = 0;
    int max_pages = m->numPages / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    for (i = 0; i < max_pages; i++) {
        if ((page_mask & (1 << i)) == 0)
            continue;
        if (seq0 != 0 && seq0_len[i] != 0) {
            for (j = 0; j < seq0_len[i]; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + seq0[i][j];
                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur] + 0;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + 0;
                }
                prev_ptr = m->accessPointsByWays[cur] + 0;
                count++;
            }
        }
        if (m->numProbesPerPage[0] == 0) {
            m->numProbesPerPage[0] = count;
        }
    }

    for (i = 0; i < max_pages; i++) {
        if ((page_mask & (1 << i)) == 0)
            continue;
        if (seq1 != 0 && seq1_len[i] != 0) {
            for (j = 0; j < seq1_len[i]; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + seq1[i][j];
                if (prev_ptr != 0) {
                    *(prev_ptr) = m->accessPointsByWays[cur] + 1;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + 1;
                }
                prev_ptr = m->accessPointsByWays[cur] + 1;
                count++;
            }
        }
        if (m->numProbesPerPage[0] == 0) {
            m->numProbesPerPage[0] = count;
        }
    }

    if (prev_ptr != 0)
        *(prev_ptr) = m->linkHead;
    return count;
}
#else
static void __setLinkCustomPerPage(struct mem_block * m,
        int ** boot_seq, int * boot_seq_len,
        int ** probe_seq, int * probe_seq_len,
        int ** prime_seq, int * prime_seq_len,
        int page_mask) {
    int i, j, k, l, n, round;
    PNP_ADDR prev_ptr;
    int set, line;
    int prev, cur, count = 0;
    int bgn;
    m->numProbesPerPage = malloc(sizeof(int));
    m->numProbesPerPage[0] = 0;
    bgn = 0;
    prev_ptr = 0;
    m->linkHead = 0;
    int max_pages = m->numPages / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);

    for (i = 0; i < max_pages; i++) {
        if ((page_mask & (1 << i)) == 0)
            continue;
        count = 0;
        // Boot Sequence
        if (boot_seq != 0 && boot_seq_len != 0) {
            for (j = 0; j < boot_seq_len[i]; j++) {
                cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + boot_seq[i][j];
                if (prev_ptr != 0) {
                    PNP_ADDR_NEXT(prev_ptr) = m->accessPointsByWays[cur] + 0;
                }
                if (m->linkHead == 0) {
                    m->linkHead = m->accessPointsByWays[cur] + 0;
                }
                prev_ptr = m->accessPointsByWays[cur] + 0;
                count++;
            }
        }
        // Probe Sequence
        for (j = 0; j < probe_seq_len[i]; j++) {
            cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + probe_seq[i][j];
            if (prev_ptr != 0) {
                PNP_ADDR_NEXT(prev_ptr) = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 1;
            }
            if (m->linkHead == 0) {
                m->linkHead = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 1;
            }
            prev_ptr = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 1;
            count++;
        }
        // Prime Sequence
        for (j = 0; j < prime_seq_len[i]; j++) {
            cur = i * N_LINES_PER_PAGE * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS) + prime_seq[i][j];
            if (prev_ptr != 0) {
                PNP_ADDR_NEXT(prev_ptr) = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 2;
            }
            if (m->linkHead == 0) {
                m->linkHead = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 2;
            }
            prev_ptr = m->accessPointsByWays[cur] + PNP_ADDR_SIZE * 2;
            count++;
        }
        if (m->numProbesPerPage[0] == 0) {
            m->numProbesPerPage[0] = count;
            // FPRINTF_ERR"%d %d %d %d\n", i, PNP_PAGE_OFFSET, count, adaptive_total_len[PNP_PAGE_OFFSET]);
        }
    }
    if (prev_ptr != 0)
        PNP_ADDR_NEXT(prev_ptr) = m->linkHead;
}
#endif

static void setLinkCustom(struct mem_block * m,
        int * boot_seq, int boot_seq_len,
        int * probe_seq, int probe_seq_len,
        int * prime_seq, int prime_seq_len) {
    int max_pages = m->numPages / (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS);
    __setLinkCustom(m, boot_seq, boot_seq_len, probe_seq, probe_seq_len, prime_seq, prime_seq_len, (1 << max_pages) - 1);
}

static void setLink(struct mem_block * m) {
#ifdef MEM_ALLOC_DEFAULT_LINK
    if (m->linkMode & LINK_MODE_BYSETS) {
        setLinkBySets(m);
    } else if (m->linkMode & LINK_MODE_EXTRA_PRIME) {
        setLinkExtraPrime(m);
    } else if (m->linkMode & LINK_MODE_PRIME_LAST) {
        setLinkPrimeLast(m);
    } else {  // Default (m->linkMode & LINK_MODE_BY_WAYS)
        setLinkByWays(m);
    }
#endif
}

#ifdef MEM_ALLOC_SHUFFLE
static void shuffleSwap(struct mem_block *m, int i, int j, int l, int n) {
    int x, y, t, k;
    int * temp;
    k = 0;
    while (k < N_OF_WAYS) {
        // Swap j l
        x = (i * n * N_OF_WAYS) + (l * N_OF_WAYS) + k;
        y = (i * n * N_OF_WAYS) + (j * N_OF_WAYS) + k;
        // printf("    %d <-> %d\n", x ,y);
        if (m->linkMode & LINK_MODE_BYSETS) {
            // TODO: This part is not correct for x & y values
            t = m->shuffleMap[x];
            m->shuffleMap[x] = m->shuffleMap[y];
            m->shuffleMap[y] = t;
            temp = m->accessPoints[x];
            m->accessPoints[x] = m->accessPoints[y];
            m->accessPoints[y] = temp;
        } else { // Default: (m->linkMode & LINK_MODE_BYWAYS)
            t = m->shuffleMap[x];
            m->shuffleMap[x] = m->shuffleMap[y];
            m->shuffleMap[y] = t;
            temp = m->accessPointsByWays[x];
            m->accessPointsByWays[x] = m->accessPointsByWays[y];
            m->accessPointsByWays[y] = temp;
        }
        k = k + 1;
    }
}

static void shuffleAccessPoints(struct mem_block * m) {
    // printMemAccessPoints(m);
    if (m->linkMode & LINK_MODE_LINEAR)
        return;
    if (m->numCacheLines <= 1)
        return;
    int i, j, k, l;
    int x, y, t;
    int * temp;
    int n;
    int cur_pos[N_LINES_PER_PAGE] = {};
    int cur_map[N_LINES_PER_PAGE] = {};
    int tmp;
    i = 0;
    if (m->linkMode & LINK_MODE_REVERSE) {
        n = (int) sysconf(_SC_PAGESIZE) / CACHE_LINE_SIZE;
        while (i < m->numPages / N_OF_WAYS) {
            l = 0;
            while (l < n / 2) {
                j = n - l - 1;
                // printf("%d <-> %d (%#10x) \n", i * n + j , i * n + l, m->accessPoints[0x40]);
                shuffleSwap(m, i, j, l, n);
                l = l + 1;
            }
            i = i + 1;
        }
    } else if (m->linkMode & LINK_MODE_RANDOM || m->linkMode & LINK_MODE_PSEUDO_RANDOM) {
        n = (int) sysconf(_SC_PAGESIZE) / CACHE_LINE_SIZE;
        if (m->linkMode & LINK_MODE_RANDOM) {
            for (i = 0; i < N_LINES_PER_PAGE; i++) {
                shuffle_vector[i] = i;
            }
            srand(456);
            for (i = (N_LINES_PER_PAGE - 1); i >= 1; i--) {
                j = rand() % (i + 1);
                tmp = shuffle_vector[i];
                shuffle_vector[i] = shuffle_vector[j];
                shuffle_vector[j] = tmp;
            }
        }
        while (i < m->numPages / N_OF_WAYS) {
            for (j = 0; j < N_LINES_PER_PAGE; j++) {
                cur_pos[j] = j;
                cur_map[j] = j;
            }
            for (l = 0; l < n; l++) {
                if (shuffle_vector[l] == -1)
                    continue;
                j = cur_pos[shuffle_vector[l]];
                shuffleSwap(m, i, j, l, n);
                cur_pos[cur_map[l]] = j;
                cur_pos[shuffle_vector[l]] = l;
                tmp = cur_map[j];
                cur_map[j] = cur_map[l];
                cur_map[l] = tmp;
            }
            i = i + 1;
        }
    }
    if (m->linkMode & LINK_MODE_SHUFFLE_LINES) {
        k = 0;
        while (k < (m->numCacheLines / N_OF_WAYS)) {
            i = 0;
            while (i < N_OF_WAYS - 1) {
                j = i + rand() / (RAND_MAX / (N_OF_WAYS - i) + 1);
                x = k * N_OF_WAYS + i;
                y = k * N_OF_WAYS + j;
                if (m->linkMode & LINK_MODE_BYSETS) {
                    t = m->shuffleMap[x];
                    m->shuffleMap[x] = m->shuffleMap[y];
                    m->shuffleMap[y] = t;
                    temp = m->accessPoints[x];
                    m->accessPoints[x] = m->accessPoints[y];
                    m->accessPoints[y] = temp;
                } else { // Default: (m->linkMode & LINK_MODE_BYWAYS)
                    t = m->shuffleMap[x];
                    m->shuffleMap[x] = m->shuffleMap[y];
                    m->shuffleMap[y] = t;
                    temp = m->accessPointsByWays[x];
                    m->accessPointsByWays[x] = m->accessPointsByWays[y];
                    m->accessPointsByWays[y] = temp;
                }
                i = i + 1;
            }
            k = k + 1;
        }
    }
}
#endif

static int indexHashing(PNP_ADDR index) {
    PNP_ADDR ret = index;
    // int hash1[] = { 31, 30, 29, 27, 25, 24, 21, 19, 18 };
    // int hash2[] = { 31, 29, 28, 26, 24, 23, 22, 21, 20, 19, 17 };
    int hash1[] = { 34, 33, 31, 29, 28, 26, 24, 23, 22, 21, 20, 19, 17, 15, 13 };
    int hash2[] = { 33, 32, 30, 28, 27, 26, 25, 24, 22, 20, 18, 17, 16, 14, 12 };
    // int hash1[] = {};
    // int hash2[] = { 34, 33, 31, 29, 28, 26, 24, 23, 22, 21, 20, 19, 17, 15, 13 };
    // int hash3[] = { 33, 32, 30, 28, 27, 26, 25, 24, 22, 20, 18, 17, 16, 14, 12 };
    int hash1_len = 15;
    int hash2_len = 15;
    int hash3_len = 15;
    int bit1, bit2, bit3;
    int bit1_pos = INDEX_BITS + OFFSET_BITS - 1;
    int bit2_pos = INDEX_BITS + OFFSET_BITS - 2;
    int bit3_pos = INDEX_BITS + OFFSET_BITS - 3;
    int i;
    // First bit
    bit1 = 0;
    for (i = 0; i < hash1_len; i++) {
        bit1 = bit1 ^ (GET_BIT(index, hash1[i]));
    }
    if (bit1 == 1)
        SET_BIT(ret, bit1_pos);
    else
        CLEAR_BIT(ret, bit1_pos);
    // Second bit
    bit2 = 0;
    for (i = 0; i < hash2_len; i++) {
        bit2 = bit2 ^ (GET_BIT(index, hash2[i]));
    }
    if (bit2 == 1)
        SET_BIT(ret, bit2_pos);
    else
        CLEAR_BIT(ret, bit2_pos);
    return ret;
}

static struct mem_block allocateMem(int size, int mode) {
    struct mem_block ret;
    int pagesize = (int) sysconf(_SC_PAGESIZE);
    int offsetBits = NUM_BITS_INTEGER - 1 - __builtin_clz(pagesize);
    // int numBits = NUM_BITS_INTEGER - 1 - __builtin_clz(size) - offsetBits;
    int numBits = NUM_BITS_INTEGER - (__builtin_clz(size - 1)) - offsetBits;
    int mask = ((1 << numBits) - 1) << offsetBits;
    int pagemask = ~(pagesize - 1);
    int cl_per_page = (int) sysconf(_SC_PAGESIZE) / CACHE_LINE_SIZE;

    ret.linkMode = mode;
    ret.curRound = 0;
    ret.numBits = numBits;
    ret.numPages = size / pagesize;
    if (size % pagesize != 0)
        ret.numPages = ret.numPages + 1;
    ret.numBlocksUsed = 0;
    ret.numBlocksMax = ret.numPages * MAX_ALLOC_COEFFICIENT
                / NUM_PAGES_PER_BLOCK;
    ret.numCacheLines = cl_per_page * ret.numPages;

    if (ret.linkMode & LINK_MODE_EXTRA_PRIME) {
        ret.numBits = NUM_BITS_INTEGER - (__builtin_clz(size * 2 - 1)) - offsetBits;;
        ret.numPages = size / N_OF_WAYS * (LINK_MODE_EXTRA_PRIME_WAYS + N_OF_WAYS) / pagesize;
        ret.numBlocksMax = ret.numPages * MAX_ALLOC_COEFFICIENT / NUM_PAGES_PER_BLOCK;
        ret.numCacheLines = cl_per_page * ret.numPages;
        numBits += 1;
        mask = ((1 << numBits) - 1) << offsetBits;
    } else if (ret.linkMode & LINK_MODE_4MB_EXTRA) {
        ret.numBits = NUM_BITS_INTEGER - (__builtin_clz(4 * 1024 * 1024 - 1)) - offsetBits;;
        ret.numPages = 4 * 1024 * 1024 / pagesize;
        ret.numBlocksMax = ret.numPages * MAX_ALLOC_COEFFICIENT / NUM_PAGES_PER_BLOCK;
        ret.numCacheLines = cl_per_page * ret.numPages;
        numBits = ret.numBits;
        mask = ((1 << numBits) - 1) << offsetBits;
    }

    ret.mapping = malloc(ret.numPages * sizeof(char*));
    memset(ret.mapping, 0, ret.numPages * sizeof(char*));
    ret.mem = malloc(ret.numPages * sizeof(char*) * MAX_ALLOC_COEFFICIENT / NUM_PAGES_PER_BLOCK);
    ret.accessPoints = malloc(ret.numCacheLines * sizeof(int*));
    ret.accessPointsByWays = malloc(ret.numCacheLines * sizeof(int*));
    ret.shuffleMap = malloc(ret.numCacheLines * sizeof(int));

    // FPRINTF_ERR"%d,%d,%d\n", ret.numBits, ret.numPages, ret.numCacheLines);

    int i, j, k, index, count;
    unsigned long vaddr, paddr, hashed_paddr;
    char * bgn_addr;
    i = 0;
    count = 0;
    while (i < ret.numBlocksMax) {
        // FPRINTF_ERR"%d %d\n", i, ret.numBlocksMax);
        // We've successfully covered everything
        if (count >= ret.numPages) {
            // FPRINTF_ERR"Memory allocation finished in %d rounds.\n", i);
            ret.numBlocksUsed = i;
            break;
        }
        // Allocate a new block
        ret.mem[i] = malloc(pagesize * (NUM_PAGES_PER_BLOCK + 1));
        bgn_addr = ((long) ret.mem[i] & pagemask) + pagesize;
        memset(ret.mem[i], 0, pagesize * (NUM_PAGES_PER_BLOCK + 1));

        uint64_t * paddrs = malloc(NUM_PAGES_PER_BLOCK * sizeof(uint64_t));
        uint64_t * vaddrs = malloc(NUM_PAGES_PER_BLOCK * sizeof(uint64_t));
        j = 0;
        while (j < NUM_PAGES_PER_BLOCK) {
            vaddrs[j] = bgn_addr + j * pagesize;
            j = j + 1;
        }
        // printf("Reading pagemap.\n");
        getPhyAddrs(vaddrs, paddrs, NUM_PAGES_PER_BLOCK);
        j = 0;
        while (j < NUM_PAGES_PER_BLOCK) {
            vaddr = vaddrs[j];
            paddr = paddrs[j];
            hashed_paddr = indexHashing(paddr);
            index = (hashed_paddr & mask) >> offsetBits;
            // if (ret.linkMode & LINK_MODE_4MB_EXTRA)
            //     FPRINTF_ERR"%#10x -> %#10x -> %#10x\n", (int)vaddr, (int)paddr, index);
            if (index >= 0 && index < ret.numPages && ret.mapping[index] == 0) {
                ret.mapping[index] = vaddr;
                count = count + 1;
            }
            j = j + 1;
        }
        free(vaddrs);
        free(paddrs);
        i = i + 1;
    }

    // Setup access points
    char * addr;
    i = 0;
    while (i < ret.numPages) {
        j = 0;
        while (j < cl_per_page) {
            index = i * cl_per_page + j;
            addr = ret.mapping[i] + (j << OFFSET_BITS);
            ret.accessPoints[index] = addr;
            j = j + 1;
        }
        i = i + 1;
    }

    // Setup access points ordered by ways
    i = 0;
    int cs_mask = (1 << INDEX_BITS) - 1;
    while (i < ret.numCacheLines) {
        j = ((i & cs_mask) * N_OF_WAYS) + (i >> INDEX_BITS);
        if (ret.linkMode & LINK_MODE_EXTRA_PRIME) {
            j = ((i & cs_mask) * (N_OF_WAYS + LINK_MODE_EXTRA_PRIME_WAYS)) + (i >> INDEX_BITS);
        }
        // printf("%#10x -> %#10x\n", i, j);
        ret.accessPointsByWays[j] = ret.accessPoints[i];
        i = i + 1;
    }

    // Default shuffle map
    i = 0;
    while (i < ret.numCacheLines) {
        ret.shuffleMap[i] = i;
        i = i + 1;
    }

    // Set Links
    // printMemPages(&ret);
    // printMemAccessPoints(&ret);
#ifdef MEM_ALLOC_SHUFFLE
    shuffleAccessPoints(&ret);
#endif
    // printMemAccessPoints(&ret);
    setLink(&ret);
    // printMemAccessPoints(&ret);

    // FPRINTF_ERR"%d,%d,%d,%#10x,%#10x\n", size, numBits, offsetBits, mask, pagemask);
    return ret;
}

static void __walkMem(struct mem_block *m, int n_lines) {
    int i;
    PNP_ADDR ptr = m->linkHead;
    for (i = 0; i < n_lines; i++) {
        ptr = PNP_ADDR_NEXT(ptr);
    }
}
static void walkMem(struct mem_block *m) {
    __walkMem(m, m->numCacheLines);
}

static void setLinkByWays(struct mem_block * m) {
    int i, j, k, n, round;
    int prev, cur, count;
    int bgn;
    for (i = 0; i < m->numCacheLines - 1; i++) {
        PNP_ADDR_NEXT(m->accessPointsByWays[i]) = m->accessPointsByWays[i + 1];
    }
    PNP_ADDR_NEXT(m->accessPointsByWays[i]) = m->accessPointsByWays[0];
    m->linkTailIndex = i;
    m->linkHead = m->accessPointsByWays[0];
}

static void freeMem(struct mem_block * m) {
    int i;
    i = 0;
    while (i < m->numBlocksUsed) {
        // FPRINTF_ERR"%d %#10x %d\n", i, m->mem[i], m->numBlocksUsed);
        free(m->mem[i]);
        i = i + 1;
    }
    free(m->shuffleMap);
    free(m->mapping);
    free(m->accessPoints);
    free(m->accessPointsByWays);
    free(m->mem);
}

#endif
