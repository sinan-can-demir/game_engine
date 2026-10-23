#ifndef PLAT_H
#define PLAT_H

#include <stdint.h>

/* Monotonic clock in nanoseconds. Starts at an arbitrary point (not wall-clock
 * time), never goes backwards. Only differences between two calls are meaningful. */
uint64_t plat_time_ns(void);

#endif /* PLAT_H */
