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

void dump(good_deque_t* d)
{
    printf("\n------------------------------------------\n");

    good_deque_t* n = good_deque_head(d);
    while(1)
    {
        if(!n)
            break;

        printf("%d\n",*GOOD_PTR2PTR(int,n->data,0));

        n = good_deque_next(n);
    }

    printf("\n------------------------------------------\n");
}

int main(int argc, char **argv)
{

    good_deque_t d;
    memset(&d,0,sizeof(d));

    good_deque_t* n = good_deque_alloc2(sizeof(int));

    *GOOD_PTR2PTR(int,n->data,0) = 1;

    good_deque_push_front(&d,n);

    n = good_deque_alloc2(1000);

    *GOOD_PTR2PTR(int,n->data,0) = 2;

    good_deque_push_front(&d,n);

    n = good_deque_alloc2(1000);

    *GOOD_PTR2PTR(int,n->data,0) = 3;

    good_deque_push_front(&d,n);

    dump(&d);

    n = good_deque_alloc2(1000);

    *GOOD_PTR2PTR(int,n->data,0) = 4;

    good_deque_push_back(&d,n);

    good_deque_t* n2 = good_deque_alloc2(1000);

    *GOOD_PTR2PTR(int,n2->data,0) = 5;

    good_deque_insert(&d,n2,n);

    n2 = good_deque_alloc2(1000);

    *GOOD_PTR2PTR(int,n2->data,0) = 6;

    good_deque_insert(&d,n2,n);

    good_deque_detach(&d,n);

    dump(&d);

    good_deque_default_clear(&d);

    return 0;
}