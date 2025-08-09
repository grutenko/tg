#define _POSIX_C_SOURCE 199309L

#include <string.h>
#include <sys/types.h>

#include "mtp-conn.h"
#include "endian.h"
#include "iobuf.h"

#include <stdint.h>
#include <sys/socket.h>
#include <time.h>

static uint64_t __msgidgen()
{
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        return (((uint64_t)ts.tv_sec << 32) |
                (((uint64_t)ts.tv_nsec << 32) / 1000000000)) &
               ~0x3ULL;
}

int mtp_conn_send(struct mtp_conn *conn)
{
        conn->out_hdr.status = MTP_STATUS_OK;
        conn->out_hdr.msg_id = __msgidgen();
        conn->out_hdr.msg_len = iobuf_len(&conn->out_buffer) + 4;
        conn->out_hdr.auth_key_id = conn->auth_key_id;

        if (iobuf_len(&conn->out_buffer) > MTP_FRAME_MAXLEN)
        {
                conn->out_hdr.status = MTP_STATUS_ERR_PAYLOAD_TOO_LONG;
                iobuf_clear(&conn->out_buffer);

                return 0;
        }

        conn->last_msg_id = conn->out_hdr.msg_id;

        if (!conn->transp.w_begin(&conn->transp,
                                  iobuf_len(&conn->out_buffer) + 20))
                goto io_error;

        uint64_t tmp64;
        uint32_t tmp32;
        uint8_t hdr[20];
        tmp64 = u64_le(conn->out_hdr.auth_key_id);
        memcpy(hdr, &tmp64, sizeof(uint64_t));
        tmp64 = u64_le(conn->out_hdr.msg_id);
        memcpy(hdr + 8, &tmp64, sizeof(uint64_t));
        tmp32 = u32_le(conn->out_hdr.msg_len);
        memcpy(hdr + 16, &tmp32, sizeof(uint32_t));

        if (!mtp_transp_write(&conn->transp, hdr, 20))
                goto io_error;

        if (!mtp_transp_write(&conn->transp, conn->out_buffer.p,
                              iobuf_len(&conn->out_buffer)))
                goto io_error;

        if (!mtp_transp_wend(&conn->transp))
                goto io_error;

        iobuf_clear(&conn->out_buffer);

        return 1;

io_error:
        conn->out_hdr.status = MTP_STATUS_ERR_IO;
        goto error;

error:
        iobuf_clear(&conn->out_buffer);

        return 0;
}

int mtp_conn_recv(struct mtp_conn *conn) {}