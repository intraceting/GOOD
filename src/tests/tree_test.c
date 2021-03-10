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
#include "libutil/tree.h"
#include "libutil/buffer.h"



int main(int argc, char **argv)
{

    good_tree_t d;
    memset(&d,0,sizeof(d));

    good_tree_t* n = good_tree_alloc2(sizeof(int));

    *GOOD_PTR2PTR(int,n->data,0) = 1;

    good_tree_insert(&d,n,NULL);

    n = good_tree_alloc2(1000);

    *GOOD_PTR2PTR(int,n->data,0) = 2;

    good_tree_insert(&d,n,NULL);

    n = good_tree_alloc2(1000);

    *GOOD_PTR2PTR(int,n->data,0) = 3;

    good_tree_insert(&d,n,NULL);


    return 0;
}