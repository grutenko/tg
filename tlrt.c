#include "tlrt.h"

#include <stdint.h>

union native
{
        int32_t _int;
        int64_t _long;
        double _double;
        uint8_t *string;
};

struct tl_obj_arg
{
        /**
         * Offset in schema->args table
         */
        int arg;
        union native native;
        size_t native_len;
        struct tl_obj_arg *next;
        struct tl_obj_arg *prev;
        struct tl_obj *obj;
};

struct tl_obj
{
        struct tl_schema *schema;
        /**
         * Offset in schema->constructors table
         */
        int ctr;
        struct tl_obj_arg *args;
        size_t argc;
};