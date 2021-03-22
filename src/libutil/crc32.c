/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "crc32.h"

int _good_crc32_init(void *opaque)
{
    uint32_t *table = (uint32_t *)opaque;

    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++)
    {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++)
        {
            if (c & 1)
                c = 0xEDB88320L ^ (c >> 1);
            else
                c = c >> 1;
        }

        table[i] = c;
    }

    return 0;
}

uint32_t good_crc32_sum(const void *data,size_t size,uint32_t old)
{
    static atomic_int init = 0;
    static uint32_t table[256] = {0};

    uint32_t sum = ~old;

    assert(data != NULL && size > 0);

    assert(good_once(&init, _good_crc32_init, table) >= 0);

    for (size_t i = 0; i < size; i++)
    {
        sum = table[(sum ^ *GOOD_PTR2PTR(uint8_t, data,i)) & 0xFF] ^ (sum >> 8);
    }

    return ~sum;
}