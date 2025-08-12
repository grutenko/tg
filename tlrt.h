#ifndef tlrt_h
#define tlrt_h

#include "iobuf.h"

#include <stdint.h>
#include <stdlib.h>

struct tl_obj;

#define TLIMPL_KIND_CTR 0x1

struct tl_type_impl
{
        int kind;
        int type_or_ctr;
        /**
         * Array of offsets in tl_type_impl table for inner types
         */
        int type_generic_impl[8];
        int type_generic_impl_len;
};

#define TLCTR_FLAG_FUNC 0x1
#define TLCTR_FLAG_NATIVE 0x2

struct tl_constructor
{
        int flags;
        /**
         * Offset ub tl_type_impl table
         */
        int type_impl;

        /**
         * crc32 of this constructor
         */
        uint32_t id;
        const char *name;

        /**
         * Count of arguments
         */
        size_t argc;
};

#define TL_ARG_FLAG_OPTIONAL 0x2

struct tl_arg
{
        int flags;
        /**
         * Offset in constructors table for this arg
         */
        int ctr;
        const char *name;
        /**
         * Offset in tl_arg array into flag:# argument
         */
        int flag_arg;

        /**
         * Bit offset in flag
         */
        int flag_offset;

        int type_impl;
};

struct tl_type
{
        const char *name;
};

/**
 * Compilation of all tables for use in runtime
 */
struct tl_schema
{
        const struct tl_constructor *constructors;
        const struct tl_arg *args;
        const struct tl_type *types;
        const struct tl_type_impl *type_impl;
        size_t constructors_len;
        size_t args_len;
        size_t types_len;
        size_t type_impl_len;
};

/**
 * Create new tl_obj with target constructor and schema
 */
struct tl_obj *tl_new(struct tl_schema *schema, int ctr);

/**
 * Return index of argument with this name or -1 if name is unknown
 */
int tl_arg_index(struct tl_obj, const char *name);

/**
 * Set value into argument
 */
int tl_arg_set(struct tl_obj *obj, int index, struct tl_obj *v);

/**
 *
 */
struct tl_obj *tl_arg_get(struct tl_obj *obj, int index);

/**
 *
 */
int tl_set_native_int(struct tl_obj *obj, int index, int32_t v);

/**
 *
 */
int tl_set_native_long(struct tl_obj *obj, int index, int64_t v);

/**
 *
 */
int tl_set_native_double(struct tl_obj *obj, int index, double v);

/**
 *
 */
int tl_set_native_string(struct tl_obj *obj, int index, int8_t *v, size_t len);

/**
 * Encode tl object
 */
int tl_encode(struct tl_obj *obj, struct iobuf *b);

/**
 * 
 */
void tl_destroy(struct tl_obj *obj);

#endif