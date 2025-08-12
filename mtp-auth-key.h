#ifndef mtp_auth_key_h
#define mtp_auth_key_h

#include "mtp-conn.h"

#include <stdint.h>

int mtp_make_session(struct mtp_conn *conn, uint8_t *auth_key);

#endif