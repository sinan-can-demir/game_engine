#include "plat.h"

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    uint64_t prev = plat_time_ns();

    for (int i = 0; i < 5; i++) {
        uint64_t now = plat_time_ns();
        printf("tick %d: now=%" PRIu64 " ns, delta=%" PRIu64 " ns\n", i, now, now - prev);
        prev = now;
    }
    return 0;
}
