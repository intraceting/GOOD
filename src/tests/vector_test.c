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

    assert(good_vector_init(&v,100));

    good_tree_t* n =  good_vector_seek(&v,0);

    n->size = 100;
    n->data = good_buffer_alloc2(100);


    good_vector_destroy(&v);
    

    return 0;
}