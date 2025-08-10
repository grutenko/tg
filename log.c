#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static int tg_debug_enable = 0;

void tg_enable_debug(int enable)
{
        tg_debug_enable = !!enable;
}

void tglogv(const char *fmt, va_list v)
{
        vprintf(fmt, v);
}

void tglog(const char *fmt, ...)
{
        va_list v;
        va_start(v, fmt);
        tglogv(fmt, v);
        va_end(v);
}

void tgdebug(const char *fmt, ...)
{
        if (tg_debug_enable)
                return;

        va_list v;
        va_start(v, fmt);
        tglogv(fmt, v);
        va_end(v);
}

void tgabort_impl(const char *file, int line, const char *fmt, ...)
{
        printf("Abort %s:%d: ", file, line);
        va_list v;
        va_start(v, fmt);
        tglogv(fmt, v);
        va_end(v);

        abort();
}