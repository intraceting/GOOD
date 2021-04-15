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
#include "libutil/general.h"
#include "libutil/signal.h"

static int lock_fd = -1;
static char lock_file[] = {"/tmp/test_process.lock"};
void exit_befor()
{
    if(lock_fd>=0)
        good_closep(&lock_fd);

    remove(lock_file);
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

    return (info->si_signo==SIGINT?-1:1);
}

int main(int argc, char **argv)
{
    char buf1[PATH_MAX]={0};

    assert(good_proc_pathfile(buf1));

    printf("%s\n",buf1);

    char buf2[PATH_MAX]={0};

    assert(good_proc_dirname(buf2,"append.name"));

    printf("%s\n",buf2);

    char buf3[PATH_MAX]={0};

    assert(good_proc_basename(buf3));

    printf("%s\n",buf3);

    char buf4[PATH_MAX]={0};

    assert(good_user_dirname(buf4,"user.name"));

    printf("%s\n",buf4);

    int pid = -1;
    lock_fd = good_proc_singleton(lock_file,&pid);
    if (lock_fd >= 0)
    {
        atexit(exit_befor);

        printf("进程(PID=%d)运行中……，Ctrl + c 结束。\n",pid);

        good_signal_t sig;
        sigfillset(&sig.signals);
        //sigdelset(&sig.signals,SIGINT);
        sig.signal_cb = signal_cb;
        sig.opaque = NULL;

        good_sigwaitinfo(&sig, -1);
    }
    else
    {
        printf("进程(PID=%d)已经运行。\n",pid);
    }

        

    return 0;
}