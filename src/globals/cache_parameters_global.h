#ifndef _CACHE_PARAMETERS_GLOBAL_H_
#define _CACHE_PARAMETERS_GLOBAL_H_

#ifndef PAGE_SIZE
#define PAGE_SIZE sysconf(_SC_PAGESIZE)
#endif

static int N_OF_CACHE_SETS = 1024;
static int CACHE_LINE_SIZE = 64;
static int N_OF_WAYS = 16;
static int CACHE_SIZE = N_OF_CACHE_SETS * CACHE_LINE_SIZE * N_OF_WAYS;
static int N_LINES_PER_PAGE = PAGE_SIZE / CACHE_LINE_SIZE;
static int CACHE_SET_STEP = N_OF_CACHE_SETS * CACHE_LINE_SIZE;
static int NUM_PAGES = N_OF_CACHE_SETS * CACHE_LINE_SIZE / PAGE_SIZE;
static int WAY_BITS = sizeof(int) * 8 - __builtin_clz(N_OF_WAYS) - 1;
static int OFFSET_BITS = sizeof(int) * 8 - __builtin_clz(CACHE_LINE_SIZE) - 1;
static int INDEX_BITS = sizeof(int) * 8 - __builtin_clz(N_OF_CACHE_SETS) - 1;
static int INDEX_OFFSET_BITS = INDEX_BITS + OFFSET_BITS;

static void loadCacheParams() {
    FILE * f;
    char fn[100];
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int level = 4;
    for (level = 4; level >= 0; level--) {
        // CL Size
        sprintf(fn, "/sys/devices/system/cpu/cpu0/cache/index%d/coherency_line_size", level);
        f = fopen(fn, "r");
        if (f == NULL) { // Check if level exist
            continue;
        }
        read = getline(&line, &len, f);
        sscanf(line, "%d", &CACHE_LINE_SIZE);
        fclose(f);
        // CL Size
        sprintf(fn, "/sys/devices/system/cpu/cpu0/cache/index%d/number_of_sets", level);
        f = fopen(fn, "r");
        read = getline(&line, &len, f);
        sscanf(line, "%d", &N_OF_CACHE_SETS);
        fclose(f);
        // CL Size
        sprintf(fn, "/sys/devices/system/cpu/cpu0/cache/index%d/ways_of_associativity", level);
        f = fopen(fn, "r");
        read = getline(&line, &len, f);
        sscanf(line, "%d", &N_OF_WAYS);
        fclose(f);
        CACHE_SIZE = N_OF_CACHE_SETS * CACHE_LINE_SIZE * N_OF_WAYS;
        N_LINES_PER_PAGE = PAGE_SIZE / CACHE_LINE_SIZE;
        CACHE_SET_STEP = N_OF_CACHE_SETS * CACHE_LINE_SIZE;
        NUM_PAGES = N_OF_CACHE_SETS * CACHE_LINE_SIZE / PAGE_SIZE;
        WAY_BITS = sizeof(int) * 8 - __builtin_clz(N_OF_WAYS) - 1;
        OFFSET_BITS = sizeof(int) * 8 - __builtin_clz(CACHE_LINE_SIZE) - 1;
        INDEX_BITS = sizeof(int) * 8 - __builtin_clz(N_OF_CACHE_SETS) - 1;
        INDEX_OFFSET_BITS = INDEX_BITS + OFFSET_BITS;
        break;
    }
    printf("LLC_LEVEL:\t%d\n", level);
    printf("N_OF_CACHE_SETS:\t%d\n", N_OF_CACHE_SETS);
    printf("CACHE_LINE_SIZE:\t%d\n", CACHE_LINE_SIZE);
    printf("N_OF_WAYS:\t%d\n", N_OF_WAYS);
    printf("CACHE_SIZE:\t%d\n", CACHE_SIZE);
    printf("N_LINES_PER_PAGE:\t%d\n", N_LINES_PER_PAGE);
    printf("CACHE_SET_STEP:\t%d\n", CACHE_SET_STEP);
    printf("NUM_PAGES:\t%d\n", NUM_PAGES);
    printf("WAY_BITS:\t%d\n", WAY_BITS);
    printf("OFFSET_BITS:\t%d\n", OFFSET_BITS);
    printf("INDEX_BITS:\t%d\n", INDEX_BITS);
    printf("INDEX_OFFSET_BITS:\t%d\n", INDEX_OFFSET_BITS);
}

#endif
