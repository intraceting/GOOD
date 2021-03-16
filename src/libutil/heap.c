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
        return;

    good_heap_free(*data);
    *data = NULL;
}