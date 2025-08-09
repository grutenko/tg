#include "transport.h"
#include "endian.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MTP_TRANSPORT_ABRIDGED_PACKET 0xEF

static int send_all(int fd, const uint8_t *b, size_t l)
{
        int rc;
        uint8_t *p = (uint8_t *)b;

        while (l)
        {
                rc = send(fd, p, l, 0);
                if (rc < 0)
                {
                        if (errno == EINTR)
                                continue;

                        return 0;
                }

                l -= rc;
                p += rc;
        }

        return 1;
}

static int recv_all(int fd, void *b, size_t l)
{
        int rc;
        uint8_t *p = b;

        while (l)
        {
                rc = recv(fd, p, l, 0);
                if (rc < 0)
                {
                        if (errno == EINTR)
                                continue;

                        return 0;
                }
                else if (rc == 0)
                        return 0;

                l -= rc;
                p += rc;
        }

        return 1;
}

static int mtp_abridged_w_begin_impl(struct mtp_transp *t, size_t len)
{
        size_t templen = len / 4;
        uint8_t b[4];
        if (templen < 0x7F)
        {
                b[0] = templen & 0xFF;

                if (send_all(t->fd, b, 1) < 0)
                        return 0;
        }
        else
        {
                b[0] = 0x7F;
                b[1] = templen & 0xFF;
                b[2] = (templen >> 8) & 0xFF;
                b[3] = (templen >> 16) & 0xFF;

                if (send_all(t->fd, b, 4) < 0)
                        return 0;
        }

        t->len = len;
        return 1;
}

static int mtp_abridged_write_impl(struct mtp_transp *t, const uint8_t *b, size_t len)
{
        if (send_all(t->fd, b, len) < 0)
                return 0;

        return 1;
}

static int mtp_abridged_w_end_impl(struct mtp_transp *t)
{
        uint8_t b[4];
        memset(b, 0, 4);

        size_t pad = (4 - (t->len % 4)) % 4;
        if (pad)
        {
                if (send_all(t->fd, b, pad) < 0)
                        return 0;
        }

        return 1;
}

static int mtp_abridged_r_begin_impl(struct mtp_transp *t)
{
        uint8_t b[4];
        if (!recv_all(t->fd, b, 1))
                return 0;

        if (b[0] < 0x7F)
        {
                t->len = b[0] * 4;
        }
        else
        {
                if (!recv_all(t->fd, b, 3))
                        return 0;

                t->len = (b[0] | (b[1] << 8) | (b[2] << 16)) * 4;
        }

        return 1;
}

static int mtp_abridged_read_impl(struct mtp_transp *t, uint8_t *b, size_t len)
{
        if (!recv_all(t->fd, b, len))
                return 0;

        return 1;
}

static int mtp_abridged_r_end_impl(struct mtp_transp *t)
{
        uint8_t b[4];

        size_t pad = (4 - (t->len % 4)) % 4;

        if (!recv_all(t->fd, b, pad))
                return 0;

        return 1;
}

static int mtp_abridged_open(struct mtp_transp *t, const char *host, int flags)
{
        int rc;
        int fd;
        fd = socket(AF_INET, SOCK_STREAM, 0);

        if (fd == -1)
                return 0;

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_addr.s_addr = inet_addr(host);
        addr.sin_family = AF_INET;
        addr.sin_port = u16_be(443);

        if (addr.sin_addr.s_addr == INADDR_NONE)
                goto error;

        rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));

        if (rc < 0)
                goto error;

        t->fd = fd;
        t->r_begin = mtp_abridged_r_begin_impl;
        t->read = mtp_abridged_read_impl;
        t->r_end = mtp_abridged_r_end_impl;
        t->w_begin = mtp_abridged_w_begin_impl;
        t->write = mtp_abridged_write_impl;
        t->w_end = mtp_abridged_w_end_impl;

        uint8_t byte = MTP_TRANSPORT_ABRIDGED_PACKET;
        rc = send(fd, &byte, 1, 0);

        if (rc < 0)
                goto error;

        return 1;
error:
        close(fd);
        return 0;
}

int mtp_transp_open(struct mtp_transp *t, int mode, const char *host, int flags)
{
        switch (mode)
        {
        case MTP_TRANSP_ABRIDGED:
                return mtp_abridged_open(t, host, flags);
        default:
                return 0;
        }
}
