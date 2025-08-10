#define _POSIX_C_SOURCE 199309L

#include "mtp-conn.h"
#include "endian.h"
#include "iobuf.h"
#include "mtp-msg-id.h"

#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>

static inline uint64_t read_u64le(const void *src)
{
        uint64_t val;
        memcpy(&val, src, sizeof(val));
        return u64_le(val);
}

static inline uint32_t read_u32le(const void *src)
{
        uint32_t val;
        memcpy(&val, src, sizeof(val));
        return u32_le(val);
}

static inline void write_u64le(void *dst, uint64_t v)
{
        v = u64_le(v);
        memcpy(dst, &v, sizeof(uint64_t));
}

static inline void write_u32le(void *dst, uint32_t v)
{
        v = u32_le(v);
        memcpy(dst, &v, sizeof(uint32_t));
}

int mtp_conn_send(struct mtp_conn *conn)
{
        conn->out_hdr.status = MTP_STATUS_OK;
        conn->out_hdr.msg_id = msgidgen();
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

        /**
         * Create MTProto message header
         */

        uint8_t hdr[20];
        write_u64le(hdr, conn->out_hdr.auth_key_id);
        write_u64le(hdr + 8, conn->out_hdr.msg_id);
        write_u32le(hdr + 16, conn->out_hdr.msg_len);

        if (!mtp_transp_write(&conn->transp, hdr, 20))
                goto io_error;

        /**
         * Send payload
         */

        if (!mtp_transp_write(&conn->transp, conn->out_buffer.p,
                              iobuf_len(&conn->out_buffer)))
                goto io_error;

        /**
         * Correct padding, etc...
         */

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

static inline size_t min(size_t a, size_t b)
{
        return a > b ? b : a;
}

int mtp_conn_recv(struct mtp_conn *conn)
{
        memset(&conn->in_hdr, 0, sizeof(struct mtp_hdr));
        iobuf_clear(&conn->in_buffer);

        if (!mtp_transp_rbegin(&conn->transp))
                goto io_error;

        if (conn->transp.len <= 20)
                goto malformed;

        if (conn->transp.len > MTP_FRAME_MAXLEN)
                goto payload_too_long;

        /**
         * Read MTProto header (20bytes)
         */

        uint8_t hdr[20];
        if (!mtp_transp_read(&conn->transp, hdr, 20))
                goto io_error;

        conn->in_hdr.auth_key_id = read_u64le(hdr);
        conn->in_hdr.msg_id = read_u64le(hdr + 8);
        conn->in_hdr.msg_len = read_u32le(hdr + 16);

        if (conn->in_hdr.msg_len <= 4)
                goto malformed;

        conn->in_hdr.msg_len -= 4;

        /**
         * Read mtproto payload
         */

        uint8_t b[256];
        size_t reserved = conn->in_hdr.msg_len;
        size_t part = 256;
        while (reserved)
        {
                part = min(256, reserved);

                if (!mtp_transp_read(&conn->transp, b, part))
                        goto io_error_clear;

                if (!iobuf_write(&conn->in_buffer, b, part))
                        goto mem_error_clear;

                reserved -= part;
        }

        if (!mtp_transp_rend(&conn->transp))
                goto io_error_clear;

        return 1;

malformed:
        conn->in_hdr.status = MTP_STATUS_MALFORMED;
        goto error;

payload_too_long:
        conn->in_hdr.status = MTP_STATUS_ERR_PAYLOAD_TOO_LONG;
        goto error;

io_error_clear:
        iobuf_clear(&conn->in_buffer);
        goto io_error;

mem_error_clear:
        conn->in_hdr.status = MTP_STATUS_ERR_MEM;
        iobuf_clear(&conn->in_buffer);
        return 0;

io_error:
        conn->in_hdr.status = MTP_STATUS_ERR_IO;
        goto error;

error:
        return 0;
}