#ifndef base64_h
#define base64_h

#include <stdint.h>
#include <stdlib.h>

struct base64_ctx
{
        uint8_t b[4];
        int bc;
        int seen_eq;
};

void base64_init(struct base64_ctx *ctx);

/**
 * Decode binary data from base64 representation. Return length of result data
 * or -1 if data invalid.
 */
int base64_decode(struct base64_ctx *ctx, uint8_t *b, size_t len,
                      uint8_t *out);

#endif