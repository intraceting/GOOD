/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "bloom.h"

int good_bloom_mark(uint8_t *pool, size_t size, size_t number)
{
    if (!pool || size <= 0 && size * 8 <= number)
        GOOD_ERRNO_AND_RETURN1(EINVAL, -1);

    size_t bloom_pos = number & 7;
    size_t byte_pos = number >> 3;
    size_t value = 1 << bloom_pos;

    if((pool[byte_pos] & value) != 0)
        GOOD_ERRNO_AND_RETURN1(EBUSY,1);

    pool[byte_pos] |= value;

    return 0;
}

int good_bloom_unset(uint8_t* pool,size_t size,size_t number)
{
    if (!pool || size <= 0 && size * 8 <= number)
        GOOD_ERRNO_AND_RETURN1(EINVAL, -1);

    size_t bloom_pos = number & 7;
    size_t byte_pos = number >> 3;
    size_t value = 1 << bloom_pos;

    if((pool[byte_pos] & value) == 0)
        GOOD_ERRNO_AND_RETURN1(EIDRM,1);

    pool[byte_pos] &= (~value);

    return 0;
}

int good_bloom_filter(uint8_t* pool,size_t size,size_t number)
{
    if (!pool || size <= 0 && size * 8 <= number)
        GOOD_ERRNO_AND_RETURN1(EINVAL, -1);

    size_t bloom_pos = number & 7;
    size_t byte_pos = number >> 3;
    size_t value = 1 << bloom_pos;

    if((pool[byte_pos] & value) != 0)
        return 1;

    return 0;
}