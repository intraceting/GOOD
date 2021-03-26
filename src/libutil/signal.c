/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "signal.h"

int good_sigwaitinfo(sigset_t *sig, time_t timeout,
                     int (*signal_cb)(const siginfo_t *info, void *opaque), 
                     void *opaque)
{
    sigset_t old;
    struct timespec tout;
    siginfo_t info;
    int chk;

    assert(sig);

    chk = pthread_sigmask(SIG_BLOCK, sig, &old);
    if (chk != 0)
        return chk;

    while (1)
    {
        if(timeout>=0)
        {
            tout.tv_sec = timeout / 1000;
            tout.tv_nsec = (timeout % 1000) * 1000000;
            chk = sigtimedwait(sig, &info, &tout);
        }
        else
        {
            chk = sigwaitinfo(sig, &info);
        }

        if (chk == -1)
            break;
        
        if(signal_cb)
            chk = signal_cb(&info,opaque);

        /*
         * 0 结束。
        */
        if(!chk)
            break;
    }
    
    pthread_sigmask(SIG_BLOCK,&old,NULL);

    return chk;
}