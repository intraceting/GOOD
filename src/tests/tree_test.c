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
#include "goodutil/tree.h"
#include "goodutil/buffer.h"

int dump(size_t deep, good_tree_t *node, void *opaque)
{
    good_tree_fprintf(stderr,deep,node,"%lu\n",node->alloc->sizes[0]);

    // if(deep>=1)
    //     return 0;
    return 1;
}

void traversal(good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_iterator_t it = {0,dump,NULL};

    good_tree_scan(root,&it);

 //   good_heap_freep((void **)&it.stack);

    printf("\n-------------------------------------\n");
}

int dump2(size_t deep, good_tree_t *node, void *opaque)
{
    good_tree_fprintf(stderr,deep,node,"%d\n",*GOOD_PTR2PTR(int,node->alloc->pptrs[0],0));

    // if(deep>=1)
    //     return 0;
    return 1;
}

void traversal2(good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_iterator_t it = {0,dump2,NULL};

    good_tree_scan(root,&it);

 //   good_heap_freep((void **)&it.stack);

    printf("\n-------------------------------------\n");
}

void test_tree()
{
    good_tree_t *d = good_tree_alloc(NULL);

    d->alloc = good_allocator_alloc2(1);

    good_tree_t *n = good_tree_alloc(NULL);

    n->alloc = good_allocator_alloc2(2);

    good_tree_insert2(d, n,1);

    good_tree_t *n2 = n = good_tree_alloc(NULL);

    n->alloc = good_allocator_alloc2(3);

    good_tree_insert2(d, n,1);

    n = good_tree_alloc(NULL);

    n->alloc = good_allocator_alloc2(4);

    good_tree_insert2(d, n,1);

    traversal(d);

    good_tree_t *m = good_tree_alloc(NULL);

    m->alloc = good_allocator_alloc2(5);

    good_tree_insert2(n, m,0);

    traversal(d);

    m = good_tree_alloc(NULL);

    m->alloc = good_allocator_alloc2(6);

    good_tree_insert2(n, m,0);

    good_tree_t *m6 = m = good_tree_alloc(NULL);

    m->alloc = good_allocator_alloc2(7);

    good_tree_insert2(n, m,0);

    good_tree_t *k = good_tree_alloc(NULL);

    k->alloc = good_allocator_alloc2(8);

    good_tree_insert2(m, k,0);

    k = good_tree_alloc(NULL);

    k->alloc = good_allocator_alloc2(9);

    good_tree_insert2(m, k,0);

    good_tree_t *u = good_tree_alloc(NULL);

    u->alloc = good_allocator_alloc2(10);

    good_tree_insert(m, u, k);

    traversal(d);

    good_tree_unlink(m6);

    traversal(d);

    //good_tree_insert_least(d, m6);
    good_tree_insert(d, m6, n2);

    traversal(d);

    good_tree_unlink(m6);
    good_tree_free(&m6);

    traversal(d);


    good_tree_free(&d);
}


int compare_cb(const good_tree_t *node1, const good_tree_t *node2, void *opaque)
{
    int src = *GOOD_PTR2PTR(int, node1->alloc->pptrs[0], 0);
    int dst = *GOOD_PTR2PTR(int, node2->alloc->pptrs[0], 0);
    if( src > dst )
        return 1;
    if( src < dst )
        return -1;

    return 0;
}

void test_sort(good_tree_t *t,int by)
{
    good_tree_order_t o = {by,compare_cb,NULL};

    good_tree_sort(t,&o);
}


void test_swap()
{
    good_tree_t *d1 = good_tree_alloc3(sizeof(int));
    *GOOD_PTR2PTR(int,d1->alloc->pptrs[0],0) = 1;

    good_tree_t *d2 = good_tree_alloc3(sizeof(int));
    *GOOD_PTR2PTR(int,d2->alloc->pptrs[0],0) = 2;

    good_tree_t *d3 = good_tree_alloc3(sizeof(int));
    *GOOD_PTR2PTR(int,d3->alloc->pptrs[0],0) = 3;

    good_tree_t *d4 = good_tree_alloc3(sizeof(int));
    *GOOD_PTR2PTR(int,d4->alloc->pptrs[0],0) = 4;

    good_tree_t *d5 = good_tree_alloc3(sizeof(int));
    *GOOD_PTR2PTR(int,d5->alloc->pptrs[0],0) = 5;

    good_tree_t *d6 = good_tree_alloc3(sizeof(int));
    *GOOD_PTR2PTR(int,d6->alloc->pptrs[0],0) = 6;

    good_tree_insert(d1,d2,NULL);
    good_tree_insert(d1,d3,NULL);
    good_tree_insert(d1,d4,NULL);
    good_tree_insert(d1,d5,NULL);
    good_tree_insert(d1,d6,NULL);

    traversal2(d1);

    good_tree_swap(d2,d3);

    traversal2(d1);

    good_tree_swap(d2,d5);

    traversal2(d1);

    good_tree_swap(d6,d2);

    traversal2(d1);

    test_sort(d1,0);

    traversal2(d1);

    test_sort(d1,1);

    traversal2(d1);

    good_tree_free(&d1);
}

int main(int argc, char **argv)
{

    test_tree();

    test_swap();

    return 0;
}