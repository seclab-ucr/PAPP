#ifndef _SCHEDULER_GLOBALS_H_
#define _SCHEDULER_GLOBALS_H_

/**
 * Slowdown parameters
 */
#ifndef NTHREADS
#define NTHREADS    1024
#endif

static int BUSY_FACTOR = 1;
//#ifndef DELAY
//#define DELAY       1000000
//#endif
static int DELAY_ADAPT_PNP = 8000000;
static int DELAY = 9000000;//900000;
// static int DELAY = 4000000;
static int DELAY_SHORT = 10000;
// static int DELAY_FLUSH_RELOAD = 200000;
static int DELAY_FLUSH_RELOAD = 9000;

//#ifndef BUSY
//#define BUSY        990000
//#endif
static int BUSY_ADAPT_PNP = 7900000;
static int BUSY = 8950000;//887000;
// static int BUSY = 3975000;
static int BUSY_SHORT = 5000;
// static int BUSY_FLUSH_RELOAD = 192000;
static int BUSY_FLUSH_RELOAD = 0;

/*
 * Scheduler
 */

#define NTHREADS 1024

#define BUSY_FACTOR 1

static pid_t scheduler_pid;

enum scheduler_status {
    TEST,
    IDLE,
    ADAPT_PNP_SAMPLING_BEGIN,
    ADAPT_PNP_SAMPLING,
    ADAPT_PNP_SAMPLING_SLOW_BEGIN,
    ADAPT_PNP_SAMPLING_SLOW,
    ADAPT_PNP_TRIMMING_BGN,
    ADAPT_PNP_TRIMMING,
    ADAPT_PNP_EVALUATE,
    WAIT_FILL_PATTERN,
    FILL_PATTERN,
    FLUSH_RELOAD_INIT,
    WAITING_KEY_STROKE,
    KEY_STROKE,
    PRINTING,
    EXIT,
    PNP_RUN
};

static enum scheduler_status cur_scheduler_status = WAITING_KEY_STROKE;

static uint64_t scheduler_current_iter = 0;

#define ADAPT_PNP_DETECTION_RATE 0.99
#define ADAPT_PNP_NOISE_LEVEL 0.5

#define SCHEDULER_ADAPT_PNP_MAX_ROUNDS 16
#define SCHEDULER_ADAPT_PNP_PNP_PER_ROUND 1000
static int scheduler_adapt_pnp_rounds = 0;
#ifndef scheduler_adapt_max_vacancy
// static int scheduler_adapt_max_vacancy = 2;
#define scheduler_adapt_max_vacancy 2
#endif

static int scheduler_adapt_cur_set = 0;

static timer_t timers[NTHREADS];
static pthread_t tids[NTHREADS];
static pthread_t victim_tid;
static volatile int timer_ready = 0;

sigset_t sigSet;
// struct timespec t_delay = { .tv_sec = 0, .tv_nsec = DELAY };

static struct mem_block sched_pnp_m;
static struct mem_block sched_victim_m;
static int ** sched_pnp_linkHead;
static int ** sched_pnp_linkTail;

enum victim_status {
    VICTIM_INIT,
    VICTIM_IDLE,
    VICTIM_RUNNING,
    VICTIM_EXIT
};

static enum victim_status cur_victim_status = VICTIM_INIT;

#endif
