/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "libutil/buffer.h"



int main(int argc, char **argv)
{
    void* p = good_buffer_alloc(1000);

    // void* p2 = good_buffer_refer(p);

    // void* p3 = good_buffer_refer(p2);

    sprintf(p,"aaa");

    printf("%s\n",(char*)p);

    good_buffer_unref(&p);

    // printf("%s\n",(char*)p2);

    
    // good_buffer_unref(&p2);


    // size_t len = good_buffer_size(p3);

    // printf("%lu,%s\n",len,(char*)p3);

    // good_buffer_unref(&p3);
    

    return 0;
}