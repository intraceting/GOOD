/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "libutil/thread.h"

void* specific_cb(void* args)
{
    good_thread_setname("haha");

    good_specific_t t;
    memset(&t,0,sizeof(t));

    t.size = 1000;
    void *p = good_specific_value(&t);

    void *p2 = good_specific_value(&t);

    assert(p == p2);

  //  good_specific_destroy(&t);

  //  sleep(1000);

    return NULL;
}

int main(int argc, char **argv)
{
    good_thread_setname("hehe");

    good_mutex_t m;
    good_mutex_init2(&m,1);

    good_mutex_lock(&m,0);

    good_mutex_unlock(&m);

    good_mutex_destroy(&m);

    good_thread_t p;
    good_thread_create2(&p,specific_cb,NULL);
    good_thread_join(&p);

    return 0;
}