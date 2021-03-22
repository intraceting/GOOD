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
#include "libutil/crc32.h"
#include "libutil/clock.h"

void* specific_cb(void* args)
{
    good_thread_setname("haha");

    printf("dot:%lu\n",good_clock_dot(NULL));

    uint32_t sum = good_crc32_sum("abc",3,0);

    printf("sun=%u,%08X\n",sum,sum);

    printf("step:%lu\n",good_clock_step(NULL));

    sum = good_crc32_sum("abc",3,sum);

    printf("sun=%u,%08X\n",sum,sum);

    printf("step:%lu\n",good_clock_step(NULL));

    sleep(3);

    printf("dot:%lu\n",good_clock_dot(NULL));

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
    p.routine = specific_cb;
    good_thread_create(&p,1);
    good_thread_join(&p);

    return 0;
}