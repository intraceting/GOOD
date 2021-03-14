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
#include "libutil/vector.h"
#include "libutil/buffer.h"


int main(int argc, char **argv)
{
    good_vector_t v = {0};

    assert(good_vector_init(&v,100)==0);

    for (int i = 0; i < 100; i++)
    {
        good_tree_t *n = good_vector_seek(&v, i);

        n->buf = good_buffer_alloc2(sizeof(int));

        *GOOD_PTR2PTR(int,n->buf->data,0) = i;
    }

    for (int i = 0; i < 10000; i++)
    {
        good_tree_t *n = good_vector_find(&v, &i,sizeof(int),1);

        n->buf = good_buffer_alloc2(sizeof(int));

        *GOOD_PTR2PTR(int,n->buf->data,0) = 10000-i;
    }

    for (int i = 0; i < 100; i++)
    {
        good_tree_t *n = good_vector_seek(&v, i);

        good_tree_t *n2 = good_vector_find(&v, &i, sizeof(int), 0);

        printf("n=%d\n", *GOOD_PTR2PTR(int,n->buf->data,0));
    }

    for (int i = 0; i < 10000; i++)
    {

        good_tree_t *n2 = good_vector_find(&v, &i, sizeof(int), 0);

        printf("n2=%d\n",*GOOD_PTR2PTR(int,n2->buf->data,0));
    }


    good_vector_destroy(&v);
    

    return 0;
}