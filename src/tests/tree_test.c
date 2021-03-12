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

int dump(size_t deep, const good_tree_t *node, void *opaque)
{
    good_tree_fprintf(stderr,deep,node,"%u\n",*GOOD_PTR2PTR(int,node->data,0));

    return 1;
}

void traversal(const good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_iterator it;

    it.stack_size = 20;
    it.stack = (good_tree_t **)good_buffer_alloc2(sizeof(good_tree_t *) * it.stack_size);
    it.dump_cb = dump;

    good_tree_traversal(root, &it);

    good_buffer_unref((void **)&it.stack);

    printf("\n-------------------------------------\n");
}

int main(int argc, char **argv)
{

    good_tree_t *d = good_tree_alloc(1000);

    good_tree_t *n = good_tree_alloc(sizeof(int));

    *(GOOD_PTR2PTR(int,n->data,0)) = 1;

    good_tree_insert2(d, n,1);

    good_tree_t *n2 = n = good_tree_alloc(1000);

    *(GOOD_PTR2PTR(int,n->data,0)) = 2;

    good_tree_insert2(d, n,1);

    n = good_tree_alloc(1000);

    *(GOOD_PTR2PTR(int,n->data,0)) = 3;

    good_tree_insert2(d, n,1);

    good_tree_t *m = good_tree_alloc(sizeof(int));

    *(GOOD_PTR2PTR(int,m->data,0)) = 4;

    good_tree_insert2(n, m,0);

    m = good_tree_alloc(sizeof(int));

    *(GOOD_PTR2PTR(int,m->data,0))  = 5;

    good_tree_insert2(n, m,0);

    good_tree_t *m6 = m = good_tree_alloc(sizeof(int));

    *(GOOD_PTR2PTR(int,m->data,0))  = 6;

    good_tree_insert2(n, m,0);

    good_tree_t *k = good_tree_alloc(sizeof(int));

    *(GOOD_PTR2PTR(int,k->data,0)) = 7;

    good_tree_insert2(m, k,0);

    k = good_tree_alloc(sizeof(int));

    *(GOOD_PTR2PTR(int,k->data,0)) = 8;

    good_tree_insert2(m, k,0);

    good_tree_t *u = good_tree_alloc(sizeof(int));

    *GOOD_PTR2PTR(int,u->data,0) = 9;

    good_tree_insert(m, u, k);

    traversal(d);

    good_tree_unlink(m6);

    traversal(d);

    //good_tree_insert_least(d, m6);
    good_tree_insert(d, m6, n2);

    traversal(d);

    good_tree_clear2(m6);

    traversal(d);

    good_tree_clear2(d);

    good_tree_free(&d);

    return 0;
}