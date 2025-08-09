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
};

int mtp_transp_open(struct mtp_transp *t, int type, const char *host,
                    int flags);

int mtp_transp_rbegin(struct mtp_transp *t);

int mtp_transp_read(struct mtp_transp *t, uint8_t *b, size_t len);

int mtp_transp_rend(struct mtp_transp *t);

int mtp_transp_wbegin(struct mtp_transp *t, size_t len);

int mtp_transp_write(struct mtp_transp *t, uint8_t *b, size_t len);

int mtp_transp_wend(struct mtp_transp *t);

#endif