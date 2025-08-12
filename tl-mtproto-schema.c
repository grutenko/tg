#include "tlrt.h"

enum
{
        TLCTR_int = 0,
        TLCTR_long = 1,
        TLCTR_double = 2,
        TLCTR_string = 3,
        TLCTR_int128 = 4,
        TLCTR_int256 = 5,
        TLCTR_vector = 6,
};

const struct tl_type_impl tl_mtproto_type_impl[] = {
    {TLIMPL_KIND_CTR, 0, {0}, 0}};

const struct tl_type tl_mtproto_type[] = {{"Int"},    {"Long"},   {"Double"},
                                          {"String"}, {"Int128"}, {"Int256"},
                                          {"Vector"}};

const struct tl_constructor tl_mtproto_constructors[] = {
    {TLCTR_FLAG_NATIVE, 0, 0, "int", 1},
    {TLCTR_FLAG_NATIVE, 1, 0, "long", 1},
    {TLCTR_FLAG_NATIVE, 2, 0, "double", 1},
    {TLCTR_FLAG_NATIVE, 3, 0, "string", 1},
    {0, 4, 0, "int128", 4},
    {0, 5, 0, "int256", 8},
    {0, 6, 0x1cb5c415, "vector", -1},
};

const struct tl_arg tl_mtproto_args[] = {
    {0, 0, NULL, -1, -1, -1}, {0, 1, NULL, -1, -1, -1},
    {0, 2, NULL, -1, -1, -1}, {0, 3, NULL, -1, -1, -1},
    {0, 4, NULL, -1, -1, 0},  {0, 4, NULL, -1, -1, 0},
    {0, 4, NULL, -1, -1, 0},  {0, 4, NULL, -1, -1, 0},
    {0, 5, NULL, -1, -1, 0},  {0, 5, NULL, -1, -1, 0},
    {0, 5, NULL, -1, -1, 0},  {0, 5, NULL, -1, -1, 0},
    {0, 5, NULL, -1, -1, 0},  {0, 5, NULL, -1, -1, 0},
    {0, 5, NULL, -1, -1, 0},  {0, 5, NULL, -1, -1, 0},
};

const struct tl_schema tl_mtproto_schema = {
    .constructors = tl_mtproto_constructors,
    .args = tl_mtproto_args,
    .types = tl_mtproto_type,
    .type_impl = tl_mtproto_type_impl,
    .type_impl_len = sizeof(tl_mtproto_type_impl) / sizeof(struct tl_type_impl),
    .constructors_len =
        sizeof(tl_mtproto_constructors) / sizeof(struct tl_constructor),
    .args_len = sizeof(tl_mtproto_args) / sizeof(struct tl_arg),
    .types_len = sizeof(tl_mtproto_type) / sizeof(struct tl_type)};