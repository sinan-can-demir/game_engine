/**
 * @file plat.h
 * @author Sinan Can Demir
 * @brief Public API for the platform abstraction layer (PAL).
 * @version 0.1
 * @date 2026-10-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef PLAT_H
#define PLAT_H

#include <stdint.h>

/* Monotonic clock in nanoseconds. Starts at an arbitrary point (not wall-clock
 * time), never goes backwards. Only differences between two calls are meaningful. */
uint64_t plat_time_ns(void);

#endif /* PLAT_H */
