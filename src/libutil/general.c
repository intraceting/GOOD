/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "general.h"

size_t good_size_align(size_t size,size_t align)
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
