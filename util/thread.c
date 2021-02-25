/*
 * This file is part of CWheel.
 * 
 * MIT License
 * 
 */
#include "thread.h"

void cw_mutex_destroy(cw_mutex_t *ctx)
{
    if (ctx)
        return;

    pthread_condattr_destroy(&ctx->condattr);
    pthread_cond_destroy(&ctx->cond);
    pthread_mutexattr_destroy(&ctx->mutexattr);
    pthread_mutex_destroy(&ctx->mutex);
}

void cw_mutex_init(cw_mutex_t* ctx,int shared)
{
    if (ctx)
        return;
    
    int pshared = (shared?PTHREAD_PROCESS_SHARED:PTHREAD_PROCESS_PRIVATE);

    pthread_condattr_init(&ctx->condattr);
    pthread_condattr_setclock(&ctx->condattr, CLOCK_MONOTONIC);
    pthread_condattr_setpshared(&ctx->condattr,pshared);
    pthread_cond_init(&ctx->cond, &ctx->condattr);

    pthread_mutexattr_init(&ctx->mutexattr);
    pthread_mutexattr_setpshared(&ctx->mutexattr,pshared);
    pthread_mutexattr_setrobust(&ctx->mutexattr,PTHREAD_MUTEX_ROBUST);
    pthread_mutex_init(&ctx->mutex,&ctx->mutexattr);
}

int cw_mutex_lock(cw_mutex_t *ctx, int try)
{
    int err = -1;

    if (ctx)
        return err;

    if(try)
        err = pthread_mutex_lock(&ctx->mutex);
    else 
        err = pthread_mutex_trylock(&ctx->mutex);

    /*当互斥量的拥有者异外结束时，恢复互斥量状态的一致性。*/
    if (err == EOWNERDEAD)
    {
        pthread_mutex_consistent(&ctx->mutex);
        pthread_mutex_unlock(&ctx->mutex);
        /*回调自己，重试。*/
        err = cw_mutex_lock(ctx,try);
    }    

    return err;
}

int cw_mutex_unlock(cw_mutex_t* ctx)
{
    int err = -1;

    if (ctx)
        return err;

    err = pthread_mutex_unlock(&ctx->mutex);
    
    return err;
}


int cw_mutex_wait(cw_mutex_t* ctx,int64_t timeout)
{
    int err = -1;

    if (ctx)
        return err;

    if(timeout>=0)
    {
        struct timespec sys_ts = {0};
        clock_gettime(CLOCK_MONOTONIC, &sys_ts);

        struct timespec out_ts = {0};
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


int cw_mutex_signal(cw_mutex_t* ctx,int broadcast)
{
    int err = -1;

    if (ctx)
        return err;

    if(broadcast)
        err = pthread_cond_broadcast(&ctx->cond);
    else
        err = pthread_cond_signal(&ctx->cond);
    
    return err;
}