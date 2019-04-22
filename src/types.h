#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#ifndef __ARM__
#include <cstdint>
#endif

typedef uint8_t LOG_ENTRY;

#if INTPTR_MAX == INT32_MAX
    #define ENVIRONMENT_32_BIT
    #define SYSTEM_BITS 32
typedef uint32_t REGISTER;
#elif INTPTR_MAX == INT64_MAX
    #define ENVIRONMENT_64_BIT
    #define SYSTEM_BITS 64
typedef uint64_t REGISTER;
#endif

#ifdef ENVIRONMENT_32_BIT
typedef uint32_t PNP_ADDR;
#define PNP_ADDR_SIZE 4
#else
typedef uint64_t PNP_ADDR;
#define PNP_ADDR_SIZE 8
#endif

#define PNP_ADDR_NEXT(X) *((PNP_ADDR*)X)

struct mem_block {
    PNP_ADDR * mapping;
    PNP_ADDR * mem;
    PNP_ADDR linkHead;
    int linkTailIndex;
    PNP_ADDR * accessPoints;
    PNP_ADDR * accessPointsByWays;
    int * shuffleMap;
    int * numProbesPerPage;
    int curRound;
    int linkMode;
    int numBlocksMax;
    int numCacheLines;
    int numPages;
    int numBits;
    int numBlocksUsed;
};

#endif
