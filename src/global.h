#ifndef GLOBAL_H
#define GLOBAL_H

#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "types.h"

/*
 * General options
 */
#define SIGWAIT

#define TEST_MODE_FNR
// #define TRADITIONAL_PNP

#ifndef NUM_OF_ACCESSES
#define NUM_OF_ACCESSES 1
#endif

#define NUM_BITS_INTEGER 32

#define CACHE_1MB

#ifndef USE_SLOWDOWN
#define USE_SLOWDOWN 1
#endif

#ifndef PRIME_N_PROBE
#define PRIME_N_PROBE 1
#endif

#ifndef RUN_VICTIM
#define RUN_VICTIM 0
#endif

#if defined (TEST_MODE_DRM)
#define SIGNAL_DELAY 45
#else
#define SIGNAL_DELAY 30
#endif
#define ATTACK_RUNTIME 180

/**
 * Logging
 */
enum log_type {
    START,
    END,
    FLUSH_RELOAD_ENTRY_HIT,
    FLUSH_RELOAD_EXIT_HIT,
    FLUSH_RELOAD_PREDRAW_HIT,
    FLUSH_RELOAD_DRAW_HIT,
    FLUSH_RELOAD_BOTH_HIT,
    VICTIM_START,
    VICTIM_END,
    SHORT_START,
    SHORT_END,
    VICTIM_KEY_PRESS_START,
    VICTIM_KEY_PRESS_END,
    VICTIM_KEY_PRESS_MID,
};

#ifndef LOG_SIZE
#define LOG_SIZE    100000
#endif

#ifndef LOG_SCHED
#define LOG_SCHED  1
#endif
#define LOG_RESULT LOG_SCHED

#ifdef PRIME_N_PROBE
#define LOG_PNP 1
#else
#define LOG_PNP 0
#endif

#define VERBOSE_PNP  1

#define PNP_LOG_ARRAY_LEN    51200000
#if VERBOSE_PNP
static int PNP_LOG_SIZE_PER_ROUND = 512;
#else
static int PNP_LOG_SIZE_PER_ROUND = 64;
#endif

/**
 * Memory allocation
 */
#define LINK_MODE_BIN_STEP N_LINES_PER_PAGE
#ifndef LINK_MODE_BIN_SIZE
#define LINK_MODE_BIN_SIZE 32
#endif

#include "globals/cache_parameters_global.h"

/*
 * Scheduler-related parameters
 */
#include "globals/scheduler_globals.h"

/*
 * Prime & probe variables
 */
#include "globals/prime_probe_globals.h"

/*
 * Flush & reload variables
 */
#include "globals/flush_reload_globals.h"

/*
 * Adaptive Prime & probe
 */
#include "globals/adaptive_pnp_globals.h"

#endif
