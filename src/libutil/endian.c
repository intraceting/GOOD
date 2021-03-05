/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "endian.h"

int good_endian_check(int is_big)
{
    long test = 1;

    if (is_big)
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