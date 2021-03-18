/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "hash.h"


uint32_t good_hash_bkdr(const void* data,size_t size)
{
    uint32_t seed = 131; /* 31 131 1313 13131 131313 etc.. */
    uint32_t hash = 0;

    if(!data)
        GOOD_ERRNO_AND_RETURN1(EINVAL,hash);

    for (size_t i = 0; i < size;i++)
    {
        hash = (hash * seed) + (*GOOD_PTR2PTR(uint8_t,data,i));
    }

    return hash;
}

uint64_t good_hash_bkdr64(const void* data,size_t size)
{
    uint64_t seed = 13113131; /* 31 131 1313 13131 131313 etc.. */
    uint64_t hash = 0;

    if(!data)
        GOOD_ERRNO_AND_RETURN1(EINVAL,hash);

    for (size_t i = 0; i < size;i++)
    {
        hash = (hash * seed) + (*GOOD_PTR2PTR(uint8_t,data,i));
    }

    return hash; 
}
