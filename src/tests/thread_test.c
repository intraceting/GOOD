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
#include "libutil/signal.h"

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

int signal_cb(const siginfo_t *info, void *opaque)
{
    printf("signo=%d,errno=%d,code =%d\n",info->si_signo,info->si_errno,info->si_code);

    switch(info->si_code)
    {
        case SI_USER:
        {
            printf("pid=%d,uid=%d\n",info->si_pid,info->si_uid);
        }
        break;
    }

    return 1;
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

    sigset_t sig;
    sigfillset(&sig);
    good_sigwaitinfo(&sig,-1,signal_cb,NULL);

    return 0;
}