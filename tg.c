#include "mtp-conn.h"
#include "mtp-transport.h"
#include "iobuf.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct dc
{
        const char *name;
        const char *location;
        const char *hostv4;
        const char *hostv6;
} dc_list[] = {{"DC1", "Miami", "149.154.175.53", "2001:b28:f23d:f001::a"},
               {"DC2", "Amsterdam", "149.154.167.51", "2001:67c:4e8:f002::a"},
               {"DC3", "Miami", "149.154.175.100", "2001:b28:f23d:f003::a"},
               {"DC4", "Amsterdam", "149.154.167.91", "2001:67c:4e8:f004::a"},
               {"DC5", "Singapore", "91.108.56.130", "2001:b28:f23f:f005::a"}};

int main()
{
        struct mtp_conn conn;
        memset(&conn, 0, sizeof(conn));

        if (!mtp_transp_open(&conn.transp, MTP_TRANSP_ABRIDGED,
                             dc_list[0].hostv4))
                goto error;
        if (!iobuf_init(&conn.in_buffer))
                goto error;
        if (!iobuf_init(&conn.out_buffer))
                goto error;

        return EXIT_SUCCESS;

error:
        tglog("Error %s", strerror(errno));
        return EXIT_FAILURE;
}