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


int main(int argc, char **argv)
{
   good_buffer_t *a = good_buffer_alloc(1000);

   good_buffer_printf(a,"12345678");

   good_buffer_t *b = good_buffer_copy(a);

   good_buffer_printf(b,"aaaaaa");

    printf("a={%s}\n",(char*)a->data);
    printf("b={%s}\n",(char*)b->data);

   good_buffer_freep(&a);
   good_buffer_freep(&b);

   void* c = good_heap_alloc(1000);

   good_buffer_t *d = good_heap_alloc(sizeof(good_buffer_t));

    d->data = c;
    d->size = 1000;

    good_buffer_printf(d,"aaaaaa");

    printf("d={%s}\n",(char*)d->data);

    good_buffer_freep(&d);
    good_heap_freep(&c);

    good_buffer_t *e = good_buffer_alloc(1000*100000);

    good_buffer_t *f = good_buffer_copy(e);
    good_buffer_t *g = good_buffer_copy(f);

    good_buffer_t *h = good_buffer_clone(g);

    good_buffer_freep(&e);
    good_buffer_freep(&f);

    good_buffer_freep(&g);
    good_buffer_freep(&h);

   return 0;
}