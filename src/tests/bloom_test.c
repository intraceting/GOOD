/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "libutil/general.h"


int main(int argc, char **argv)
{
    size_t s = 10000000000;
    uint8_t* buf = (uint8_t*)good_heap_alloc(s);

    for(size_t i = 0;i<s*8;i++)
    {
        assert(good_bloom_mark(buf,s,i)==0);
    }

    for(size_t i = 0;i<s;i++)
    {
        assert(buf[i]==255);
    }
    

    for(size_t i = 0;i<s*8;i++)
    {
        assert(good_bloom_filter(buf,s,i)==1);
        assert(good_bloom_unset(buf,s,i)==0);
    }

    for(size_t i = 0;i<s;i++)
    {
        assert(buf[i]==0);
    }



    good_heap_free(buf);


    return 0;
}