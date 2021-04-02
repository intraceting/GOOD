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

void test1()
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
}

void test2()
{
    good_buffer_t *a = good_buffer_alloc(10);

    printf("%lu\n",good_buffer_write(a,"aaaaaa",6));

    printf("%lu\n",good_buffer_printf(a,"%s","bb"));

    printf("%lu\n",good_buffer_fill(a,'c'));

    printf("%lu\n",good_buffer_write(a,"dddd",6));

    char buf[11] = {0};

    printf("%lu\n",good_buffer_read(a,buf,7));

    good_buffer_read_vacuum(a);

    printf("%lu\n",good_buffer_read(a,buf,7));

    good_buffer_freep(&a);
}

int main(int argc, char **argv)
{
   
 //  test1();

   test2();

   return 0;
}