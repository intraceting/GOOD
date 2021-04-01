/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "process.h"


int good_proc_singleton(const char *lockfile,int* pid)
{
    int fd = -1;
    char strpid[16] = {0};

    assert(lockfile);

    fd = good_open(lockfile, 1, 0, 1);
    if (fd < 0)
        return -1;

    /*
     * 通过尝试加独占锁来确定是否程序已经运行。
    */
    if (flock(fd, LOCK_EX | LOCK_NB) == 0)
    {
        /*
         * PID可视化，便于阅读。
        */
        snprintf(strpid,15,"%d",getpid());

        /*
         * 清空。
        */
        ftruncate(fd, 0);

        /*
         * 写入文件。
        */
        good_write(fd,strpid,strlen(strpid),NULL);
        fsync(fd);

        /*
         *进程ID就是自己。
        */
        if(pid)
           *pid = getpid();

        /*
         * 走到这里返回锁定文件的句柄。
        */
        return fd;
    }

    /*
     * 进程ID需要从锁定文件中读取。
    */
    if(pid)
    {
        good_read(fd,strpid,12,NULL);

        if(good_strtype(strpid,isdigit))
            *pid = atoi(strpid);
        else
            *pid = -1;
    }

    /*
     * 独占失败，关闭句柄，返回-1。
    */
    good_closep(&fd);
    GOOD_ERRNO_AND_RETURN1(EPERM,-1);
}
