/**
 * @file platform_linux.c
 * @author Sinan Can Demir
 * @brief 
 * @version 0.1
 * @date 2026-10-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#define _POSIX_C_SOURCE 199309L

#include "plat.h"
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

uint64_t const SEC_TO_NSEC = 1000000000;

char* str_error(){
    return strerror(errno);
}

void get_error_message(char *s){
    perror(s);
}

uint64_t plat_time_ns(void){
    struct timespec ts;  // has nanoseconds
    
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1){
        get_error_message(str_error());
    }
    uint64_t total = (ts.tv_sec * SEC_TO_NSEC) + ts.tv_nsec;

    return total;
}