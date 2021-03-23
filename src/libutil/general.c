/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "general.h"

size_t good_align(size_t size, size_t align)
{
    /*
     * 大于1时，对齐才有意义。
    */
    if (size && align > 1)
    {
        size_t padding = size % align;
        return (size + ((padding > 0) ? align - padding : 0));
    }

    return size;
}

int good_once(atomic_int *status, int (*routine)(void *opaque), void *opaque)
{
    int chk, ret;
    atomic_int cmp = 0;

    assert(status != NULL && opaque != NULL);

    if (atomic_compare_exchange_strong(status, &cmp, 1))
    {
        ret = 0;

        chk = routine(opaque);

        atomic_store(status, ((chk == 0) ? 2 : 0));
    }
    else
    {
        ret = 1;

        while (atomic_load(status) == 1)
            pthread_yield();
    }

    chk = ((atomic_load(status) == 2) ? 0 : -1);

    return (chk == 0 ? ret : -1);
}
