#include "alloc.h"
#include "log.h"

#include <stdlib.h>

void *xalloc_impl(const char *file, int line, size_t n)
{
        void *a = malloc(n);
        if (!a)
                tgabort_impl(file, line, "Cannot allocate %zu bytes.", n);

        return a;
}