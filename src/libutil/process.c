/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "process.h"

char *good_user_dirname(char *buf, const char *append)
{
    assert(buf);

    snprintf(buf, PATH_MAX, "/var/run/user/%d/", getuid());

    if (access(buf, R_OK | W_OK | X_OK | F_OK) != 0)
        GOOD_ERRNO_AND_RETURN1(ENOENT, NULL);

    if (append)
        good_dirdir(buf, append);

    return buf;
}

char *good_app_pathfile(char *buf)
{
    assert(buf);

    if (readlink("/proc/self/exe", buf, PATH_MAX) == -1)
        return NULL;

    return buf;
}

char *good_app_dirname(char *buf, const char *append)
{
    char *tmp = NULL;

    assert(buf);

    tmp = good_heap_alloc(PATH_MAX);
    if (!tmp)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, NULL);

    if (good_app_pathfile(tmp))
    {
        good_dirname(buf, tmp);

        if (append)
            good_dirdir(buf, append);
    }
    else
    {
        /*
         * 这里的覆盖不会影响调用者。
        */
        buf = NULL;
    }

    good_heap_freep((void **)&tmp);

    return buf;
}

char *good_app_basename(char *buf)
{
    char *tmp = NULL;

    assert(buf);

    tmp = good_heap_alloc(PATH_MAX);
    if (!tmp)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, NULL);

    if (good_app_pathfile(tmp))
    {
        good_basename(buf, tmp);
    }
    else
    {
        /*
         * 这里的覆盖不会影响调用者。
        */
        buf = NULL;
    }

    good_heap_freep((void **)&tmp);

    return buf;
}

int good_app_singleton(const char *lockfile,int* pid)
{
    int fd = -1;
    char strpid[12] = {0};

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
        sprintf(strpid,"%d",getpid());

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
        good_read(fd,strpid,11,NULL);

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