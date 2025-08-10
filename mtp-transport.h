#ifndef transport_h
#define transport_h

#include <stdlib.h>
#include <stdint.h>

#define MTP_TRANSP_ABRIDGED 0x1

struct mtp_transp
{
        int fd;
        size_t len;
        int status;

        /**
         * Begin sending frame by selected transport
         */
        int (*w_begin)(struct mtp_transp *t, size_t len);

        /**
         * Continue sending data into socket
         */
        int (*write)(struct mtp_transp *t, const uint8_t *b, size_t len);

        /**
         * Correct ending send frame, add padding, etc...
         */
        int (*w_end)(struct mtp_transp *t);

        /**
         * Begin reading next frame.
         */
        int (*r_begin)(struct mtp_transp *t);

        /**
         * Read next part of frame data.
         */
        int (*read)(struct mtp_transp *t, uint8_t *b, size_t len);

        /**
         * Correct ending reading frame.
         */
        int (*r_end)(struct mtp_transp *t);

        /**
         * Correct close transport
         */
        int (*close)(struct mtp_transp *t);
};

/**
 * Open connection for server use selected transport.
 */
int mtp_transp_open(struct mtp_transp *t, int type, const char *host);

/**
 * Begin frame reading frame. Read length or any.
 */
int mtp_transp_rbegin(struct mtp_transp *t);

/**
 * Read data from transport
 */
int mtp_transp_read(struct mtp_transp *t, uint8_t *b, size_t len);

/**
 * Correct close transport frame: read padding, etc...
 */
int mtp_transp_rend(struct mtp_transp *t);

/**
 * Begin frame writing. initialize field write length and any specific data
 */
int mtp_transp_wbegin(struct mtp_transp *t, size_t len);

/**
 * Write next block of data
 */
int mtp_transp_write(struct mtp_transp *t, uint8_t *b, size_t len);

/**
 * Correct end of writed frame: padding, crc32, etc transport specific data
 */
int mtp_transp_wend(struct mtp_transp *t);

/**
 * Corrent close transport
 */
int mtp_transp_close(struct mtp_transp *t);
#endif