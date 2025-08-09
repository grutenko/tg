#include "mtp-conn.h"
#include "mtp-transport.h"
#include "iobuf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
        struct mtp_conn conn;
        memset(&conn, 0, sizeof(conn));

        if (!mtp_transp_open(&conn.transp, MTP_TRANSP_ABRIDGED, "127.0.0.1"))
                goto error;
        if (!iobuf_init(&conn.in_buffer))
                goto error;
        if (!iobuf_init(&conn.out_buffer))
                goto error;

        return EXIT_SUCCESS;

error:
        perror("Error");
        return EXIT_FAILURE;
}