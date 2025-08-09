#ifndef endian_h
#define endian_h

#include <stdint.h>

int is_big_endian();
uint32_t u32_le(uint32_t a);
uint64_t u64_le(uint64_t a);
uint32_t u32_be(uint32_t a);
uint16_t u16_be(uint16_t a);

#endif