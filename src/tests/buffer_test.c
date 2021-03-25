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

void free_clean(size_t number,uint8_t **data,size_t* size, void *opaque)
{
    for(size_t i = 0;i<number;i++)
    {
        if(data[i])
            printf("[%ld]={%s}\n",i,(char*)data[i]);
    }
}

void free_free(size_t number,uint8_t **data,size_t* size, void *opaque)
{
    printf("[%ld]={%s}\n",0l,(char*)data[0]);
    
    good_heap_freep((void**)&data[0]);
}

int main(int argc, char **argv)
{
    size_t size[] = {100,45,0,0,123};
    good_buffer_t* p = good_buffer_alloc(size,5,free_clean,NULL);

    for (size_t i = 0; i < p->number; i++)
    {
        printf("size[%ld]=%ld,size1[%ld]=%ld\n",i,p->size[i],i,p->size1[i]);

        if(!p->size[i])
            continue;

        memset(p->data[i], 'A'+i, p->size[i]);
        p->size1[i] = 34;


    }

    good_buffer_t* q =good_buffer_refer(p);


    for(size_t i = 0;i<p->number;i++)
    {
        printf("size[%ld]=%ld,size1[%ld]=%ld\n",i,p->size[i],i,p->size1[i]);

        if(!p->size[i])
            continue;

        memset(GOOD_PTR2PTR(char,q->data[i],1),'G',p->size[i]-2);
    }
 
    good_buffer_unref(&p);

    good_buffer_unref(&q);


    p = good_buffer_alloc(NULL,1,free_free,NULL);

    p->data[0] = good_heap_alloc(123);


    memset(p->data[0],'C',122);

    good_buffer_unref(&p);

    p = good_buffer_alloc3(1000);

    q = good_buffer_refer(p);

    good_buffer_unref(&p);

    good_buffer_unref(&q);

    return 0;
}