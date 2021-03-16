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

uint32_t good_hash_bkdr(const void* data,size_t size)
{
    uint32_t seed = 131; /* 31 131 1313 13131 131313 etc.. */
    uint32_t hash = 0;

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

    for (size_t i = 0; i < size;i++)
    {
        hash = (hash * seed) + (*GOOD_PTR2PTR(uint8_t,data,i));
    }

    return hash; 
}

int good_endian_check(int big)
{
    long test = 1;

    if (big)
        return (*((char *)&test) != 1);

    return (*((char *)&test) == 1);
}

uint8_t *good_endian_swap(uint8_t *dst, int len)
{
    if (len == 2 || len == 3)
    {
        GOOD_INTEGER_SWAP(dst[0], dst[len - 1]);
    }
    else if (len == 4)
    {
        GOOD_INTEGER_SWAP(dst[0], dst[3]);
        GOOD_INTEGER_SWAP(dst[1], dst[2]);
    }
    else if (len == 8)
    {
        GOOD_INTEGER_SWAP(dst[0], dst[7]);
        GOOD_INTEGER_SWAP(dst[1], dst[6]);
        GOOD_INTEGER_SWAP(dst[2], dst[5]);
        GOOD_INTEGER_SWAP(dst[3], dst[4]);
    }
    else if( len > 1 )
    {
        /*
        * 5,6,7,other,...
        */
        for (int i = 0; i < len; i++)
            GOOD_INTEGER_SWAP(dst[len - i - 1], dst[i]);
    }

    return dst;
}

uint8_t* good_endian_ntoh(uint8_t* dst,int len)
{
    if(good_endian_check(0))
        return good_endian_swap(dst,len);
    
    return dst;
}

uint16_t good_endian_ntoh16(uint16_t num)
{
    return *((uint16_t*)good_endian_ntoh((uint8_t*)&num,sizeof(num)));
}

uint32_t good_endian_ntoh32(uint32_t num)
{
    return *((uint32_t*)good_endian_ntoh((uint8_t*)&num,sizeof(num)));
}

uint64_t good_endian_ntoh64(uint64_t num)
{
    return *((uint64_t*)good_endian_ntoh((uint8_t*)&num,sizeof(num)));
}

uint8_t* good_endian_hton(uint8_t* dst,int len)
{
    if (good_endian_check(0))
        return good_endian_swap(dst,len);

    return dst;
}

uint16_t good_endian_hton16(uint16_t num)
{
    return *((uint16_t *)good_endian_hton((uint8_t *)&num, sizeof(num)));
}

uint32_t good_endian_hton32(uint32_t num)
{
    return *((uint32_t *)good_endian_hton((uint8_t *)&num, sizeof(num)));
}

uint64_t good_endian_hton64(uint64_t num)
{
    return *((uint64_t *)good_endian_hton((uint8_t *)&num, sizeof(num)));
}

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