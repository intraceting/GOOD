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
    if (deep <= 0)
    {
        printf("root\n");
    }
    else
    {
        int num = *GOOD_PTR2PTR(int, node->data, 0);

        for (size_t i = 0; i < deep - 1; i++)
        {
            if ((i + 1 == deep - 1) && !good_tree_next(good_tree_father(node)))
            {
                printf("    ");
            }
            else
            {
                printf("   │");
            }
        }

        if(good_tree_next(node))
            printf("   ├── %d\n", num);
        else 
            printf("   └── %d\n", num);
    }
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

    *GOOD_PTR2PTR(int, n->data, 0) = 1;

    good_tree_push_head(d, n);

    good_tree_t *n2 = n = good_tree_alloc(1000);

    *GOOD_PTR2PTR(int, n->data, 0) = 2;

    good_tree_push_head(d, n);

    n = good_tree_alloc(1000);

    *GOOD_PTR2PTR(int, n->data, 0) = 3;

    good_tree_push_head(d, n);

    good_tree_t *m = good_tree_alloc(sizeof(int));

    *GOOD_PTR2PTR(int, m->data, 0) = 4;

    good_tree_push_tail(n, m);

    m = good_tree_alloc(sizeof(int));

    *GOOD_PTR2PTR(int, m->data, 0) = 5;

    good_tree_push_tail(n, m);

    good_tree_t *m6 = m = good_tree_alloc(sizeof(int));

    *GOOD_PTR2PTR(int, m->data, 0) = 6;

    good_tree_push_tail(n, m);

    good_tree_t *k = good_tree_alloc(sizeof(int));

    *GOOD_PTR2PTR(int, k->data, 0) = 7;

    good_tree_push_tail(m, k);

    k = good_tree_alloc(sizeof(int));

    *GOOD_PTR2PTR(int, k->data, 0) = 8;

    good_tree_push_tail(m, k);

    good_tree_t *u = good_tree_alloc(sizeof(int));

    *GOOD_PTR2PTR(int, u->data, 0) = 9;

    good_tree_insert(m, u, k);

    traversal(d);

    good_tree_detach(m6);

    traversal(d);

    //good_tree_push_tail(d, m6);
    good_tree_insert(d, m6, n2);

    traversal(d);

    good_tree_free2(&d);

    return 0;
}