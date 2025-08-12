#include "mtp-auth-key.h"
#include "iobuf.h"
#include "endian.h"
#include "mtp-conn.h"
#include "rand.h"
#include "log.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>

struct req_pq_multi
{
        uint8_t nonce[16];
};

static int req_pq_multi_encode(struct req_pq_multi *obj, struct iobuf *b)
{
        uint32_t v = u32_le(0x05162463);
        return iobuf_write(b, (uint8_t *)&v, sizeof(uint32_t)) &&
               iobuf_write(b, obj->nonce, sizeof(uint8_t) * 16);
}

struct res_pq
{
        uint8_t nonce[16];
        uint8_t server_nonce[16];
        uint8_t pq[32];
        uint64_t server_public_key_fingerprints[32];
        size_t server_public_key_fingerprints_len;
        size_t pq_len;
};

static int read_string_len(struct iobuf *b)
{
        uint8_t len8;
        if (!iobuf_read(b, &len8, 1))
                return -1;

        if (len8 <= 253)
        {
                return len8;
        }
        else if (len8 == 254)
        {
                uint8_t len_bytes[3];
                if (!iobuf_read(b, len_bytes, 3))
                        return -1;
                return len_bytes[0] | (len_bytes[1] << 8) |
                       (len_bytes[2] << 16);
        }
        return -1; // Неверный формат
}

static int res_pq_decode(struct iobuf *b, struct res_pq *obj)
{
        uint32_t id;
        if (!iobuf_read(b, (uint8_t *)&id, sizeof(uint32_t)))
                return 0;
        id = u32_le(id);
        if (id != 0x05162463)
                return 0;
        if (!iobuf_read(b, (uint8_t *)obj->nonce, sizeof(uint8_t) * 16))
                return 0;
        if (!iobuf_read(b, (uint8_t *)obj->server_nonce, sizeof(uint8_t) * 16))
                return 0;
        int pq_len = read_string_len(b);
        if (pq_len < 0 || pq_len > 32)
                return 0;
        if (!iobuf_read(b, obj->pq, pq_len))
                return 0;
        obj->pq_len = pq_len;
        uint8_t hole[4];
        size_t pad = 0ULL;
        if (pq_len <= 253)
                pad = (4 - ((pq_len + 1) % 4)) % 4;
        else
                pad = (4 - (pq_len % 4)) % 4;
        if (!iobuf_read(b, hole, pad))
                return 0;
        if (!iobuf_read(b, (uint8_t *)&id, sizeof(uint32_t)))
                return 0;
        id = u32_le(id);
        if (id != 0x1cb5c415)
                return 0;
        uint32_t pubkey_len;
        if (!iobuf_read(b, (uint8_t *)&pubkey_len, sizeof(uint32_t)))
                return 0;
        pubkey_len = u32_le(pubkey_len);
        if (pubkey_len > 32)
                return 0;
        uint32_t i;
        uint64_t pubkey;
        for (i = 0; i < pubkey_len; i++)
        {
                if (!iobuf_read(b, (uint8_t *)&pubkey, sizeof(uint64_t)))
                        return 0;

                pubkey = u64_le(pubkey);
                obj->server_public_key_fingerprints[i] = pubkey;
        }
        obj->server_public_key_fingerprints_len = pubkey_len;

        return 1;
}

int mtp_make_session(struct mtp_conn *conn, uint8_t *auth_key)
{
        struct req_pq_multi obj;
        if (!secure_random_bytes(obj.nonce, sizeof(uint8_t) * 16,
                                 RAND_FLAG_TRUE))
        {
                tglog("Cannot read random bytes: %s", strerror(errno));
                return 0;
        }
        if (!req_pq_multi_encode(&obj, &conn->out_buffer))
                TGABORT("Cannot allocate memory.");
        if (!mtp_conn_send(conn))
        {
                tglog("Connection error: %s", mtp_strerr(conn->in_hdr.status));
                return 0;
        }
        // Wait response from server
        if (!mtp_conn_recv(conn))
        {
                if (conn->in_hdr.status == MTP_STATUS_ERR_MEM)
                        TGABORT("Cannot allocate memory.");

                tglog("Connection error: %s", mtp_strerr(conn->in_hdr.status));
                return 0;
        }
        struct res_pq res;
        // decode response
        if (!res_pq_decode(&conn->in_buffer, &res))
        {
                tglog("res_pq message format malformed");
                return 0;
        }

        return 1;
}