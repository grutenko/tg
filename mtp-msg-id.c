#define _POSIX_C_SOURCE 200809L

#include "mtp-msg-id.h"

#include <stdint.h>
#include <time.h>

uint64_t msgidgen()
{
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        return (((uint64_t)ts.tv_sec << 32) |
                (((uint64_t)ts.tv_nsec << 32) / 1000000000)) &
               ~0x3ULL;
}