#ifndef log_h
#define log_h

#include <stdarg.h>

void tglogv(const char *fmt, va_list v);
void tglog(const char *fmt, ...);
void tgdebug(const char *fmt, ...);
void tgabort_impl(const char *file, int line, const char *fmt, ...);

#define TGABORT(fmt, ...)                                                      \
        tgabort_impl(__FILE__, __LINE__, (fmt), ##__VA_ARGS__);

#endif