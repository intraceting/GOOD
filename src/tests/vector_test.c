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

    good_vector_destroy(&v);
    

    return 0;
}