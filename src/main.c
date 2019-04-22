#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sched.h>
#include <unistd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "utils/utils.h"
#include "global.h"
#include "mem_alloc.h"
#include "prime_probe.h"

#include "experiments/lru_profile.h"
#include "experiments/lru_profile_custom.h"
#include "experiments/prefetch_profile.h"
#include "experiments/svf_test.h"

int main(int argc, char** argv) {
    init_utils();

    int i;
    setAffinity(3);

    THRESHOLD_BYTE = 80;

    // svf_test_main();
    // prefetch_profile_main();
    // lru_profile_main();
    // lru_profile_custom_main();
	
    return 0;
}
