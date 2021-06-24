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
#include "abtkutil/tree.h"
#include "abtkutil/buffer.h"

int dump(size_t deep, abtk_tree_t *node, void *opaque)
{
    abtk_tree_fprintf(stderr,deep,node,"%lu\n",node->alloc->sizes[0]);

    // if(deep>=1)
    //     return 0;
    return 1;
}

void traversal(abtk_tree_t *root)
{
    printf("\n-------------------------------------\n");

    abtk_tree_iterator_t it = {0,dump,NULL};

    abtk_tree_scan(root,&it);

 //   abtk_heap_freep((void **)&it.stack);

    printf("\n-------------------------------------\n");
}

int dump2(size_t deep, abtk_tree_t *node, void *opaque)
{
    abtk_tree_fprintf(stderr,deep,node,"%d\n",*ABTK_PTR2PTR(int,node->alloc->pptrs[0],0));

    // if(deep>=1)
    //     return 0;
    return 1;
}

void traversal2(abtk_tree_t *root)
{
    printf("\n-------------------------------------\n");

    abtk_tree_iterator_t it = {0,dump2,NULL};

    abtk_tree_scan(root,&it);

 //   abtk_heap_freep((void **)&it.stack);

    printf("\n-------------------------------------\n");
}

void test_tree()
{
    abtk_tree_t *d = abtk_tree_alloc(NULL);

    d->alloc = abtk_allocator_alloc2(1);

    abtk_tree_t *n = abtk_tree_alloc(NULL);

    n->alloc = abtk_allocator_alloc2(2);

    abtk_tree_insert2(d, n,1);

    abtk_tree_t *n2 = n = abtk_tree_alloc(NULL);

    n->alloc = abtk_allocator_alloc2(3);

    abtk_tree_insert2(d, n,1);

    n = abtk_tree_alloc(NULL);

    n->alloc = abtk_allocator_alloc2(4);

    abtk_tree_insert2(d, n,1);

    traversal(d);

    abtk_tree_t *m = abtk_tree_alloc(NULL);

    m->alloc = abtk_allocator_alloc2(5);

    abtk_tree_insert2(n, m,0);

    traversal(d);

    m = abtk_tree_alloc(NULL);

    m->alloc = abtk_allocator_alloc2(6);

    abtk_tree_insert2(n, m,0);

    abtk_tree_t *m6 = m = abtk_tree_alloc(NULL);

    m->alloc = abtk_allocator_alloc2(7);

    abtk_tree_insert2(n, m,0);

    abtk_tree_t *k = abtk_tree_alloc(NULL);

    k->alloc = abtk_allocator_alloc2(8);

    abtk_tree_insert2(m, k,0);

    k = abtk_tree_alloc(NULL);

    k->alloc = abtk_allocator_alloc2(9);

    abtk_tree_insert2(m, k,0);

    abtk_tree_t *u = abtk_tree_alloc(NULL);

    u->alloc = abtk_allocator_alloc2(10);

    abtk_tree_insert(m, u, k);

    traversal(d);

    abtk_tree_unlink(m6);

    traversal(d);

    //abtk_tree_insert_least(d, m6);
    abtk_tree_insert(d, m6, n2);

    traversal(d);

    abtk_tree_unlink(m6);
    abtk_tree_free(&m6);

    traversal(d);


    abtk_tree_free(&d);
}


int compare_cb(const abtk_tree_t *node1, const abtk_tree_t *node2, void *opaque)
{
    int src = *ABTK_PTR2PTR(int, node1->alloc->pptrs[0], 0);
    int dst = *ABTK_PTR2PTR(int, node2->alloc->pptrs[0], 0);
    if( src > dst )
        return 1;
    if( src < dst )
        return -1;

    return 0;
}

void test_sort(abtk_tree_t *t,int by)
{
    abtk_tree_order_t o = {by,compare_cb,NULL};

    abtk_tree_sort(t,&o);
}


void test_swap()
{
    abtk_tree_t *d1 = abtk_tree_alloc3(sizeof(int));
    *ABTK_PTR2PTR(int,d1->alloc->pptrs[0],0) = 1;

    abtk_tree_t *d2 = abtk_tree_alloc3(sizeof(int));
    *ABTK_PTR2PTR(int,d2->alloc->pptrs[0],0) = 2;

    abtk_tree_t *d3 = abtk_tree_alloc3(sizeof(int));
    *ABTK_PTR2PTR(int,d3->alloc->pptrs[0],0) = 3;

    abtk_tree_t *d4 = abtk_tree_alloc3(sizeof(int));
    *ABTK_PTR2PTR(int,d4->alloc->pptrs[0],0) = 4;

    abtk_tree_t *d5 = abtk_tree_alloc3(sizeof(int));
    *ABTK_PTR2PTR(int,d5->alloc->pptrs[0],0) = 5;

    abtk_tree_t *d6 = abtk_tree_alloc3(sizeof(int));
    *ABTK_PTR2PTR(int,d6->alloc->pptrs[0],0) = 6;

    abtk_tree_insert(d1,d2,NULL);
    abtk_tree_insert(d1,d3,NULL);
    abtk_tree_insert(d1,d4,NULL);
    abtk_tree_insert(d1,d5,NULL);
    abtk_tree_insert(d1,d6,NULL);

    traversal2(d1);

    abtk_tree_swap(d2,d3);

    traversal2(d1);

    abtk_tree_swap(d2,d5);

    traversal2(d1);

    abtk_tree_swap(d6,d2);

    traversal2(d1);

    test_sort(d1,0);

    traversal2(d1);

    test_sort(d1,1);

    traversal2(d1);

    abtk_tree_free(&d1);
}

int main(int argc, char **argv)
{

    test_tree();

    test_swap();

    return 0;
}