/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "thread.h"

void good_mutex_destroy(good_mutex_t *ctx)
{
    assert(ctx);

    pthread_condattr_destroy(&ctx->condattr);
    pthread_cond_destroy(&ctx->cond);
    pthread_mutexattr_destroy(&ctx->mutexattr);
    pthread_mutex_destroy(&ctx->mutex);

    memset(ctx,0,sizeof(*ctx));
}

void good_mutex_init(good_mutex_t *ctx)
{
    assert(ctx);

    pthread_cond_init(&ctx->cond, &ctx->condattr);
    pthread_mutex_init(&ctx->mutex, &ctx->mutexattr);
}

void good_mutex_init2(good_mutex_t* ctx,int shared)
{
    assert(ctx);

    pthread_condattr_init(&ctx->condattr);
    pthread_condattr_setclock(&ctx->condattr, CLOCK_MONOTONIC);
    pthread_condattr_setpshared(&ctx->condattr,(shared?PTHREAD_PROCESS_SHARED:PTHREAD_PROCESS_PRIVATE));

    pthread_mutexattr_init(&ctx->mutexattr);
    pthread_mutexattr_setpshared(&ctx->mutexattr,(shared?PTHREAD_PROCESS_SHARED:PTHREAD_PROCESS_PRIVATE));
    pthread_mutexattr_setrobust(&ctx->mutexattr,PTHREAD_MUTEX_ROBUST);

    good_mutex_init(ctx);
}

int good_mutex_lock(good_mutex_t *ctx, int try)
{
    int err = -1;

    if (!ctx)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    if(try)
        err = pthread_mutex_lock(&ctx->mutex);
    else 
        err = pthread_mutex_trylock(&ctx->mutex);

    /*当互斥量的拥有者异外结束时，恢复互斥量状态的一致性。*/
    if (err == EOWNERDEAD)
    {
        /**/
        pthread_mutex_consistent(&ctx->mutex);
        pthread_mutex_unlock(&ctx->mutex);
        /*回调自己，重试。*/
        err = good_mutex_lock(ctx,try);
    }    

    return err;
}

int good_mutex_unlock(good_mutex_t* ctx)
{
    int err = -1;

    if (!ctx)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    err = pthread_mutex_unlock(&ctx->mutex);
    
    return err;
}


int good_mutex_wait(good_mutex_t* ctx,int64_t timeout)
{
    int err = -1;
    struct timespec sys_ts;
    struct timespec out_ts;
    __clockid_t condclock;

    if (!ctx)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    if(timeout>=0)
    {
        err = pthread_condattr_getclock(&ctx->condattr, &condclock);
        if (err != 0)
            return err;

        if (condclock == CLOCK_MONOTONIC)
            clock_gettime(CLOCK_MONOTONIC, &sys_ts);
        else if (condclock == CLOCK_REALTIME)
            clock_gettime(CLOCK_REALTIME, &sys_ts);
        else
            GOOD_ERRNO_AND_RETURN1(EINVAL,err=-1);

        out_ts.tv_sec = sys_ts.tv_sec + (timeout / 1000);
        out_ts.tv_nsec = sys_ts.tv_nsec + (timeout % 1000);

        err = pthread_cond_timedwait(&ctx->cond,&ctx->mutex,&out_ts);
    }
    else
    {
        err = pthread_cond_wait(&ctx->cond,&ctx->mutex);
    }
    
    return err;
}


int good_mutex_signal(good_mutex_t* ctx,int broadcast)
{
    int err = -1;

    if (!ctx)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    if(broadcast)
        err = pthread_cond_broadcast(&ctx->cond);
    else
        err = pthread_cond_signal(&ctx->cond);
    
    return err;
}

int good_thread_create(good_thread_t *ctx,int joinable)
{
    int err = -1;
    pthread_attr_t attr;
  
    if (!ctx)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    if(!ctx->routine)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,(joinable?PTHREAD_CREATE_JOINABLE:PTHREAD_CREATE_DETACHED));

    err = pthread_create(&ctx->handle,&attr,ctx->routine,ctx->opaque);

    pthread_attr_destroy(&attr);

    return err;
}


int good_thread_join(good_thread_t *ctx)
{
    int err = -1;
    pthread_attr_t attr;
    int detachstate = -1;

    if (!ctx)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    err = pthread_getattr_np(ctx->handle,&attr);
    if (err != 0)
        return err;

    err = pthread_attr_getdetachstate(&attr,&detachstate);
    if (err != 0)
        return err;
    
    pthread_attr_destroy(&attr);

    if (detachstate == PTHREAD_CREATE_JOINABLE)
        err = pthread_join(ctx->handle, &ctx->result);

    return err;
}

int good_thread_setname(const char* fmt,...)
{
    int err = -1;
    char name[16] = {0};

    if(!fmt || !fmt[0])
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    va_list vaptr;
    va_start(vaptr, fmt);
    snprintf(name,16,fmt,vaptr);
    va_end(vaptr);

    err = pthread_setname_np(pthread_self(),name);

    return err;
}

int good_thread_getname(char name[16])
{
    int err = -1;

    if(!name)
        GOOD_ERRNO_AND_RETURN1(EINVAL,err);

    err = pthread_getname_np(pthread_self(),name,16);

    return err; 
}