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

void free_clean(void *buf, void *opaque)
{
    printf("%s\n",(char*)buf);
    

    memset(buf,0,100);
}

void free_free(void *buf, void *opaque)
{
    printf("%s\n",(char*)buf);
    

    good_heap_free(buf);
}

int main(int argc, char **argv)
{
    good_buffer_t* p = good_buffer_alloc(1000,free_clean,NULL);

    memset(p->data,'A',34);

    good_buffer_t* q =good_buffer_refer(p);

    memset(q->data,'B',20);
 
    good_buffer_unref(&p);

    good_buffer_unref(&q);

  
    p = good_buffer_alloc(0,free_free,NULL);

    p->data = good_heap_alloc(123);


    memset(p->data,'C',122);

    good_buffer_unref(&p);

    return 0;
}