#include "iobuf.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define IOBUF_BLOCK 128

int iobuf_init(struct iobuf *b)
{
        b->p = malloc(IOBUF_BLOCK);

        if (!b->p)
                return 0;

        b->rp = b->p;
        b->wp = b->p;
        b->alloc = IOBUF_BLOCK;

        return 1;
}

static int iobuf_reserve_if_needed(struct iobuf *b, size_t len)
{
        if (b->alloc - (b->wp - b->p) >= len)
                return 1;

        size_t needed = len - (b->alloc - (b->wp - b->p));
        size_t new_alloc = b->alloc + (IOBUF_BLOCK * (needed / IOBUF_BLOCK + 1));
        uint8_t *p = realloc(b->p, new_alloc);

        if (!p)
                return 0;

        b->rp = b->rp + (p - b->p);
        b->wp = b->wp + (p - b->p);
        b->alloc = new_alloc;
        b->p = p;

        return 1;
}

int iobuf_write(struct iobuf *b, const uint8_t *data, size_t len)
{
        int rc;
        rc = iobuf_reserve_if_needed(b, len);
        if (!rc)
                return 0;

        memcpy(b->wp, data, len);
        b->wp += len;

        return 1;
}

int iobuf_read(struct iobuf *b, uint8_t *data, size_t len)
{
        if ((size_t)(b->wp - b->rp) > len)
                return 0;

        memcpy(data, b->rp, len);
        b->rp += len;

        return 1;
}

size_t iobuf_len(struct iobuf *b)
{
        return (size_t)(b->wp - b->p);
}

void iobuf_clear(struct iobuf *b)
{
        b->rp = b->p;
        b->wp = b->p;
}

void iobuf_free(struct iobuf *b)
{
        free(b->p);
        b->p = (void *)0;
        b->rp = (void *)0;
        b->wp = (void *)0;
        b->alloc = 0ULL;
}