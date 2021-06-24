/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "abtkutil/thread.h"
#include "abtkutil/crc32.h"
#include "abtkutil/clock.h"
#include "abtkutil/signal.h"

void* specific_cb(void* args)
{
    abtk_thread_setname("haha");

    printf("dot:%lu\n",abtk_clock_dot(NULL));

    uint32_t sum = abtk_crc32_sum("abc",3,0);

    printf("sun=%u,%08X\n",sum,sum);

    printf("step:%lu\n",abtk_clock_step(NULL));

    sum = abtk_crc32_sum("abc",3,sum);

    printf("sun=%u,%08X\n",sum,sum);

    printf("step:%lu\n",abtk_clock_step(NULL));

    sleep(3);

    printf("dot:%lu\n",abtk_clock_dot(NULL));

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
    abtk_thread_setname("hehe");

    abtk_mutex_t m;
    abtk_mutex_init2(&m,1);

    abtk_mutex_lock(&m,0);

    abtk_mutex_unlock(&m);

    abtk_mutex_destroy(&m);

    abtk_thread_t p;
    p.routine = specific_cb;
    abtk_thread_create(&p,1);
    abtk_thread_join(&p);

    abtk_signal_t sig;
    sigfillset(&sig.signals);
    sig.signal_cb = signal_cb;
    sig.opaque = NULL;
     
    abtk_sigwaitinfo(&sig,-1);

    return 0;
}