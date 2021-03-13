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
#include "libutil/buffer.h"

void clean(void *buf, void *opaque)
{
    printf("%s\n",buf);
    

    memset(buf,0,100);
}

int main(int argc, char **argv)
{
    good_buffer_t* p = good_buffer_alloc(1000,clean,NULL);

    memset(p->data,'A',34);

    good_buffer_t* q =good_buffer_refer(p);

    memset(q->data,'B',20);
 
    good_buffer_unref(&p);

    good_buffer_unref(&q);

  

    return 0;
}