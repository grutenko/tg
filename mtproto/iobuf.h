#ifndef iobuf_h
#define iobuf_h

#include <stdlib.h>

struct iobuf
{
        /**
         * Pointer to start of memory block of this buffer.
         */
        uint8_t *p;
        /**
         * Head of buffer. Peek on reading
         */
        uint8_t *rp;
        /**
         * Tail of buffer. Peek on writing
         */
        uint8_t *wp;
        /**
         * Allocated size. For Expand if needed.
         */
        size_t alloc;
};

int iobuf_init(struct iobuf *b);
int iobuf_write(struct iobuf *b, const uint8_t *data, size_t len);
int iobuf_read(struct iobuf *b, uint8_t *data, size_t len);
size_t iobuf_len(struct iobuf *b);
void iobuf_clear(struct iobuf *b);
void iobuf_free(struct iobuf *b);

#endif