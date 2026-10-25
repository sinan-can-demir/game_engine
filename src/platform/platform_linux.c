/**
 * @file platform_linux.c
 * @author Sinan Can Demir
 * @brief Linux implementation of the platform abstraction layer (PAL).
 * @version 0.1
 * @date 2026-10-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#define _POSIX_C_SOURCE 199309L

#include "plat.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static uint64_t const SEC_TO_NSEC = 1000000000;  // global constant for nano second to second conversion

uint64_t plat_time_ns(void){
    struct timespec ts;  // has nanoseconds
    
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1){
        perror("clock_gettime");
        abort();
    }
    uint64_t total = (ts.tv_sec * SEC_TO_NSEC) + ts.tv_nsec;

    return total;
}