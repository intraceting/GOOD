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

void dump(size_t deep, const good_tree_t *node, void *opaque)
{
    // if (deep <= 0)
    // {
    //     printf("root\n");
    // }
    // else
    // {
    //     for (size_t i = 0; i < deep - 1; i++)
    //     {
    //         if ((i + 1 == deep - 1) && !good_tree_sibling(good_tree_father(node),0))
    //         {
    //             printf("    ");
    //         }
    //         else
    //         {
    //             printf("   │");
    //         }
    //     }

    //     if(good_tree_sibling(node,0))
    //         printf("   ├── %lu\n", node->code);
    //     else 
    //         printf("   └── %lu\n", node->code);
    // }

    good_tree_fprintf(stderr,deep,node,"%lu",node->code);
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

    n->code = 1;

    good_tree_insert_first(d, n);

    good_tree_t *n2 = n = good_tree_alloc(1000);

    n->code = 2;

    good_tree_insert_first(d, n);

    n = good_tree_alloc(1000);

    n->code = 3;

    good_tree_insert_first(d, n);

    good_tree_t *m = good_tree_alloc(sizeof(int));

    m->code = 4;

    good_tree_insert_least(n, m);

    m = good_tree_alloc(sizeof(int));

    m->code  = 5;

    good_tree_insert_least(n, m);

    good_tree_t *m6 = m = good_tree_alloc(sizeof(int));

    m->code  = 6;

    good_tree_insert_least(n, m);

    good_tree_t *k = good_tree_alloc(sizeof(int));

    k->code = 7;

    good_tree_insert_least(m, k);

    k = good_tree_alloc(sizeof(int));

    k->code = 8;

    good_tree_insert_least(m, k);

    good_tree_t *u = good_tree_alloc(sizeof(int));

    u->code = 9;

    good_tree_insert(m, u, k);

    traversal(d);

    good_tree_detach(m6);

    traversal(d);

    //good_tree_insert_least(d, m6);
    good_tree_insert(d, m6, n2);

    traversal(d);

    good_tree_free2(&d);

    return 0;
}