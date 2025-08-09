#ifndef connection_h
#define connection_h

#include "iobuf.h"
#include "mtp-transport.h"

#include <stdint.h>

#define MTP_FRAME_MAXLEN (16 * 1024 * 1024)

#define MTP_STATUS_OK 0x0
#define MTP_STATUS_ERR_PAYLOAD_TOO_LONG 0x1
#define MTP_STATUS_ERR_IO 0x2
#define MTP_STATUS_ERR_MEM 0x3
#define MTP_STATUS_MALFORMED 0x4

struct mtp_hdr
{
        int status;
        uint64_t auth_key_id;
        uint64_t msg_id;
        uint32_t msg_len;
};

struct mtp_conn
{
        struct mtp_transp transp;

        uint8_t auth_key[256];
        uint64_t auth_key_id;

        /**
         * mtp_conn functions will automatically write to mtp_hdr fields.
         */
        struct mtp_hdr in_hdr;
        struct mtp_hdr out_hdr;

        /**
         * recv and send buffers will be cleared after every mtp_recv(),
         * mtp_send()
         */
        struct iobuf in_buffer;
        struct iobuf out_buffer;

        /**
         * Last msg id for message id in header.
         */
        uint64_t last_msg_id;
};

/**
 * Send out buffer and write generated header into out_hdr
 * * Clear buffer data after send
 */
int mtp_conn_send(struct mtp_conn *conn);

/**
 * Recv frame into in buffer and parse mtproto header into in_hdr
 * * Clear in_buffer before recv
 */
int mtp_conn_recv(struct mtp_conn *conn);

#endif