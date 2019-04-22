#ifndef _PRIME_PROBE_GLOBALS_H_
#define _PRIME_PROBE_GLOBALS_H_

#define SCHEDULER_PNP_PAGES 1
#define SCHEDULER_PNP_PAGE_MASK (((1 << SCHEDULER_PNP_PAGES) - 1) << TEST_OFFSET)

#define THRESHOLD_SET 340
#define THRESHOLD_SET_BASE 200
#define THRESHOLD_SET_STEP 60
#define THRESHOLD_SET_VACANT_STEP 30

static uint64_t THRESHOLD_LINE = 100;
static uint64_t THRESHOLD_BYTE = 40;

static char* log_file_pnp;
static char* log_file_pnp_test;
static volatile unsigned int pnp_log_idx;
static LOG_ENTRY pnp_log[PNP_LOG_ARRAY_LEN] = {};
static LOG_ENTRY *pnp_log_ptr;

static volatile PNP_ADDR pnp_ptr;
static int PNP_I, PNP_J, PNP_K;
// static volatile unsigned int PNP_TS0, PNP_TS1;
static int PNP_TS0, PNP_TS1;
static char PNP_VACANTS_PTR;
static int PNP_NUM_SETS = 32;
static int PROBE_BGN;
static int PROBE_END;
static int PNP_NUM_PAGES = NUM_PAGES;
static int PNP_PAGE_OFFSET = 0;
static int PNP_PAGE_MASK = 0xffff;

#endif
