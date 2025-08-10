#ifndef alloc_h
#define alloc_h

#include <stdlib.h>

void *xalloc_impl(const char *file, int line, size_t n);

#define XALLOC(n) xalloc_impl(__FILE__, __LINE__, (n))

#endif