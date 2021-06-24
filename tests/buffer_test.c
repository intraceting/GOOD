/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "abtkutil/buffer.h"
#include "abtkutil/pool.h"

void test1()
{
    abtk_buffer_t *a = abtk_buffer_alloc2(1000);

   abtk_buffer_printf(a,"12345678");

   abtk_buffer_t *b = abtk_buffer_copy(a);

   abtk_buffer_printf(b,"aaaaaa");

    printf("a={%s}\n",(char*)a->data);
    printf("b={%s}\n",(char*)b->data);

   abtk_buffer_freep(&a);
   abtk_buffer_freep(&b);

   void* c = abtk_heap_alloc(1000);

   abtk_buffer_t *d = abtk_heap_alloc(sizeof(abtk_buffer_t));

    d->data = c;
    d->size = 1000;

    abtk_buffer_printf(d,"aaaaaa");

    printf("d={%s}\n",(char*)d->data);

    abtk_buffer_t *d2 =abtk_buffer_copy(d);

    abtk_heap_freep((void**)&d);
    abtk_heap_freep(&c);
    abtk_buffer_freep(&d2);

    abtk_buffer_t *e = abtk_buffer_alloc2(1000*100000);

    abtk_buffer_t *f = abtk_buffer_copy(e);
    abtk_buffer_t *g = abtk_buffer_copy(f);

    abtk_buffer_t *h = abtk_buffer_clone(g);

    abtk_buffer_freep(&e);
    abtk_buffer_freep(&f);

    abtk_buffer_freep(&g);
    abtk_buffer_freep(&h);
}

void test2()
{
    abtk_buffer_t *a = abtk_buffer_alloc2(10);

    printf("%lu\n",abtk_buffer_write(a,"aaaaaa",6));

    printf("%lu\n",abtk_buffer_printf(a,"%s","bb"));

    printf("\r%lu\n",abtk_buffer_fill(a,'c'));

    printf("%lu\n" ,abtk_buffer_write(a,"dddd",6));

    char buf[11] = {0};

    fprintf(stderr,"%lu\n",abtk_buffer_read(a,buf,7));

    abtk_buffer_drain(a);

    fprintf(stderr,"%lu\n",abtk_buffer_read(a,buf,7));

    fprintf(stderr,"%lu\n",abtk_buffer_write(a,"abcdefg",6));

    abtk_buffer_resize(a,1000);

    abtk_buffer_freep(&a);
}

void test3()
{
    abtk_buffer_t *a = abtk_buffer_alloc(NULL);

    abtk_buffer_t *b = abtk_buffer_copy(a);

    abtk_buffer_t *c = abtk_buffer_clone(b);

    abtk_buffer_freep(&a);
    abtk_buffer_freep(&b);
    abtk_buffer_freep(&c);
}

void test4()
{
    abtk_pool_t p = {0};

    abtk_pool_init(&p,sizeof(size_t),3);

    printf("\n---------------\n");

    size_t id = -1;
    for(int i = 0;i<3;i++)
    {
        assert(abtk_pool_pull(&p,&id,sizeof(id))<=0);
        printf("%ld\n",id);
    }

    printf("\n---------------\n");

    assert(abtk_pool_pull(&p,&id,sizeof(id)) == -1);

    id = 2;
    assert(abtk_pool_push(&p,&id,sizeof(id))>0);
    id = 1;
    assert(abtk_pool_push(&p,&id,sizeof(id))>0);
    id = 3;
    assert(abtk_pool_push(&p,&id,sizeof(id))>0);
 //   assert(abtk_pool_push(&p,&id,sizeof(id))>0);
    

    printf("\n---------------\n");

    for(int i = 0;i<p.table->numbers;i++)
    {
         assert(abtk_pool_pull(&p,&id,sizeof(id))>0);
        printf("%ld\n",id);

        assert(abtk_pool_push(&p,&id,sizeof(id))>0);
    }

    printf("\n---------------\n");

    abtk_pool_destroy(&p);
}

int main(int argc, char **argv)
{
   
   test1();

   test2();

    test3();

    test4();

   return 0;
}