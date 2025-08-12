#include "rand.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

int secure_random_bytes(uint8_t *b, size_t len, int flags)
{
        int fd, rc;
        if (flags & RAND_FLAG_TRUE)
                fd = open("/dev/random", O_RDONLY);
        else
                fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1)
                return 0;

        if (flags & RAND_FLAG_PROGRESS)
        {
                while (len)
                {
                        rc = read(fd, b, 1);
                        if (rc == -1)
                                goto error;

                        b++;
                        len--;

                        putc('.', stdout);
                }
        }
        else
        {
                while (len)
                {
                        rc = read(fd, b, len);
                        if (rc == -1)
                                goto error;

                        b += rc;
                        len -= rc;
                }
        }

        return 1;

error:
        close(fd);
        return 0;
}