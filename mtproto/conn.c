#include "conn.h"
#include "iobuf.h"
#include "endian.h"

#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdint.h>

int mtp_init(struct mtp_conn *conn, const char *host, int transport)
{
        int rc;
        memset(conn->auth_key, 0, 256);
        conn->auth_key_id = 0ULL;
        memset(&conn->in_hdr, 0, sizeof(struct mtp_hdr));
        memset(&conn->out_hdr, 0, sizeof(struct mtp_hdr));

        rc = iobuf_init(&conn->in_buffer);
        if (!rc)
                return 0;
        rc = iobuf_init(&conn->out_buffer);
        if (!rc)
        {
                iobuf_free(&conn->in_buffer);
                return 0;
        }

        conn->last_msg_id = 0ULL;
        return 1;
}

static uint64_t __msgidgen()
{
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        uint64_t seconds_part = (uint64_t)ts.tv_sec << 32;
        uint64_t fractional_part = ((uint64_t)ts.tv_nsec << 32) / 1000000000;
        uint64_t msg_id = seconds_part | fractional_part;
        msg_id &= ~0x3ULL;
        return msg_id;
}

int mtp_send(struct mtp_conn *conn)
{
        int rc;
        conn->out_hdr.status = MTP_STATUS_OK;
        conn->out_hdr.msg_id = __msgidgen();
        conn->out_hdr.msg_len = iobuf_len(&conn->out_buffer) + 4;
        conn->out_hdr.auth_key_id = conn->auth_key_id;

        if (iobuf_len(&conn->out_buffer) > MTP_FRAME_MAXLEN)
                goto payload_too_long_error;

        conn->last_msg_id = conn->out_hdr.msg_id;

        if (!conn->transp.w_begin(&conn->transp, iobuf_len(&conn->out_buffer) + 20))
                goto io_error;

        uint64_t auth_key_id = u64_le(conn->out_hdr.auth_key_id);
        if (!conn->transp.write(&conn->transp, &auth_key_id, sizeof(uint64_t)))
                goto io_error;

        uint64_t msg_id = u64_le(conn->out_hdr.msg_id);
        if (!conn->transp.write(&conn->transp, &msg_id, sizeof(uint64_t)))
                goto io_error;

        uint32_t msg_len = u32_le(conn->out_hdr.msg_len);
        if (!conn->transp.write(&conn->transp, &msg_len, sizeof(uint32_t)))
                goto io_error;

        if (!conn->transp.write(&conn->transp, conn->out_buffer.p, iobuf_len(&conn->out_buffer)))
                goto io_error;

        if (!conn->transp.w_end(&conn->transp))
                goto io_error;

        iobuf_clear(&conn->out_buffer);

        return 1;

payload_too_long_error:
        conn->out_hdr.status = MTP_STATUS_ERR_PAYLOAD_TOO_LONG;
        goto error;

io_error:
        conn->out_hdr.status = MTP_STATUS_ERR_IO;
        goto error;

error:
        iobuf_clear(&conn->out_buffer);

        return 0;
}

int mtp_recv(struct mtp_conn *conn)
{

}