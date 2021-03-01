/*
 * This file is part of CWheel.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include "libcwutil/thread.h"

void* specific_cb(void* args)
{

    cw_specific_t t;
    memset(&t,0,sizeof(t));

    t.size = 1000;
    void *p = cw_specific_value(&t);

    void *p2 = cw_specific_value(&t);

    assert(p == p2);

  //  cw_specific_destroy(&t);

    return NULL;
}

int main(int argc, char **argv)
{
    cw_mutex_t m;
    cw_mutex_init(&m,1);

    cw_mutex_lock(&m,0);

    cw_mutex_unlock(&m);

    cw_mutex_destroy(&m);

    pthread_t p ;
    pthread_create(&p,NULL,specific_cb,NULL);
    void *pret = NULL;
    pthread_join(p,&pret);

    return 0;
}