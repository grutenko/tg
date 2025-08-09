#include "endian.h"

#include <stdint.h>

int is_big_endian()
{
        unsigned int x = 1;
        return ((char *)&x)[0] == 0;
}

uint16_t u16_swap(uint16_t a)
{
        return (a >> 8) | (a << 8);
}

uint32_t u32_swap(uint32_t x)
{
        return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) |
               ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000);
}

uint64_t u64_swap(uint64_t x)
{
        return ((x >> 56) & 0x00000000000000FFULL) |
               ((x >> 40) & 0x000000000000FF00ULL) |
               ((x >> 24) & 0x0000000000FF0000ULL) |
               ((x >> 8) & 0x00000000FF000000ULL) |
               ((x << 8) & 0x000000FF00000000ULL) |
               ((x << 24) & 0x0000FF0000000000ULL) |
               ((x << 40) & 0x00FF000000000000ULL) |
               ((x << 56) & 0xFF00000000000000ULL);
}

uint32_t u32_le(uint32_t a)
{
        return is_big_endian() ? u32_swap(a) : a;
}

uint64_t u64_le(uint64_t a)
{
        return is_big_endian() ? u64_swap(a) : a;
}

uint32_t u32_be(uint32_t a)
{
        return is_big_endian() ? a : u32_swap(a);
}

uint16_t u16_be(uint16_t a)
{
        return is_big_endian() ? a : u16_swap(a);
}