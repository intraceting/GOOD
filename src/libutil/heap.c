/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "heap.h"


void *good_heap_alloc(size_t size)
{
    assert(size > 0);

    return calloc(1,size);
}

void* good_heap_realloc(void *buf,size_t size)
{
    assert(size > 0);

    return realloc(buf,size);
}

void good_heap_free(void *data)
{
    if (data)
        free(data);
}

void good_heap_freep(void **data)
{
    if (!data || !*data)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    good_heap_free(*data);
    *data = NULL;
}

void *good_heap_dup(const void *data, size_t size)
{
    void *buf = NULL;

    if (!data || size <= 0)
        GOOD_ERRNO_AND_RETURN1(EINVAL, NULL);

    buf = good_heap_alloc(size);
    if (!buf)
        GOOD_ERRNO_AND_RETURN1(EINVAL, NULL);

    memcpy(buf, data, size);

    return buf;
}