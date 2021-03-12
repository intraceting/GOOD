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
#include "libutil/deque.h"
#include "libutil/buffer.h"

void traversal(good_deque_t *q)
{
    printf("\n------------------------------\n");

    good_deque_t *p = good_deque_scroll(q, NULL, 1);
    while (p)
    {
        printf("%lu\n", p->code);

        p = good_deque_scroll(q, p, 0);
    }

    printf("\n------------------------------\n");
}

int main(int argc, char **argv)
{
    good_deque_t q = {0};

    good_deque_t *n = good_deque_alloc(0);

    n->code = 1;

    good_deque_push(&q, n, 0);

    traversal(&q);

    n = good_deque_alloc(0);

    n->code = 2;

    good_deque_push(&q, n, 0);

    traversal(&q);

    n = good_deque_alloc(0);

    n->code = 3;

    good_deque_push(&q, n, 0);

    traversal(&q);

    good_deque_t *m = good_deque_alloc(0);

    m->code = 5;

    good_deque_push(&q, m, 1);

    traversal(&q);

    m = good_deque_alloc(0);

    m->code = 6;

    good_deque_insert(&q, m, n);

    traversal(&q);

    good_deque_clear2(&q);



    return 0;
}