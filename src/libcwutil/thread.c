/*
 * This file is part of CWheel.
 * 
 * MIT License
 * 
 */
#include "thread.h"

void cw_mutex_destroy(cw_mutex_t *ctx)
{
    if (!ctx)
        return;

    pthread_condattr_destroy(&ctx->condattr);
    pthread_cond_destroy(&ctx->cond);
    pthread_mutexattr_destroy(&ctx->mutexattr);
    pthread_mutex_destroy(&ctx->mutex);

    memset(ctx,0,sizeof(*ctx));
}

void cw_mutex_init(cw_mutex_t *ctx)
{
    if (!ctx)
        return;

    pthread_cond_init(&ctx->cond, &ctx->condattr);
    pthread_mutex_init(&ctx->mutex, &ctx->mutexattr);
}

void cw_mutex_init2(cw_mutex_t* ctx,int shared)
{
    if (!ctx)
        return;

    pthread_condattr_init(&ctx->condattr);
    pthread_condattr_setclock(&ctx->condattr, CLOCK_MONOTONIC);
    pthread_condattr_setpshared(&ctx->condattr,(shared?PTHREAD_PROCESS_SHARED:PTHREAD_PROCESS_PRIVATE));

    pthread_mutexattr_init(&ctx->mutexattr);
    pthread_mutexattr_setpshared(&ctx->mutexattr,(shared?PTHREAD_PROCESS_SHARED:PTHREAD_PROCESS_PRIVATE));
    pthread_mutexattr_setrobust(&ctx->mutexattr,PTHREAD_MUTEX_ROBUST);

    cw_mutex_init(ctx);
}

int cw_mutex_lock(cw_mutex_t *ctx, int try)
{
    int err = -1;

    if (!ctx)
        return err;

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
        err = cw_mutex_lock(ctx,try);
    }    

    return err;
}

int cw_mutex_unlock(cw_mutex_t* ctx)
{
    int err = -1;

    if (!ctx)
        return err;

    err = pthread_mutex_unlock(&ctx->mutex);
    
    return err;
}


int cw_mutex_wait(cw_mutex_t* ctx,int64_t timeout)
{
    int err = -1;
    struct timespec sys_ts;
    struct timespec out_ts;
    __clockid_t condclock;

    if (!ctx)
        return err;

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
            return err = -EINVAL;

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

    if (!ctx)
        return err;

    if(broadcast)
        err = pthread_cond_broadcast(&ctx->cond);
    else
        err = pthread_cond_signal(&ctx->cond);
    
    return err;
}

void cw_specific_destroy(cw_specific_t *ctx)
{
    void * value = NULL;

    if (!ctx)
        return;

    if(atomic_load(&ctx->status)!=CW_SPECIFIC_STATUS_STABLE)
        return;
    
    value = pthread_getspecific(ctx->key);
    pthread_setspecific(ctx->key,NULL);

    /*这里只释放私有数据，KEY是多线程共享，而且系统在进程结束时会自动回收，因此不用处理。*/
    ctx->free_cb(value);

}

void* cw_specific_value(cw_specific_t*ctx)
{
    int err = 0;
    void* value = NULL;
    atomic_int status_expected = CW_SPECIFIC_STATUS_UNKNOWN;

    if (!ctx)
        return NULL;

    /*多线程初始化，这里要保护一下。*/
    if(atomic_compare_exchange_strong(&ctx->status,&status_expected,CW_SPECIFIC_STATUS_SYNCHING))
    {
        /*如果回调函数未指定，则绑定默认函数。*/
        if(!ctx->alloc_cb)
            ctx->alloc_cb = cw_specific_default_alloc;
        if(!ctx->free_cb)
            ctx->free_cb = cw_specific_default_free;

        /*创建KEY*/
        err = pthread_key_create(&ctx->key,ctx->free_cb);

        /*如果创建失败，则恢复到未知状态。*/
        atomic_store(&ctx->status,(err?CW_SPECIFIC_STATUS_UNKNOWN:CW_SPECIFIC_STATUS_STABLE));
    }
    else
    {
        /*等待同步完成。*/
        while(atomic_load(&ctx->status)==CW_SPECIFIC_STATUS_SYNCHING)
            pthread_yield();
    }
    
    /*稳定的才有效。*/
    if(atomic_load(&ctx->status)==CW_SPECIFIC_STATUS_STABLE)
    {
        value = pthread_getspecific(ctx->key);

        if(!value)
        {
            value = ctx->alloc_cb(ctx->size);

            if(value)
                pthread_setspecific(ctx->key,value);
        }
    }

    return value;
}

void* cw_specific_default_alloc(size_t s)
{
    void* m = malloc(s);

    if(m)
        memset(m,0,s);

    return m;
}

void cw_specific_default_free(void *m)
{
    if (m)
        free(m);
}

int cw_thread_create(cw_thread_t *ctx,void *(*routine)(void *user),void *user)
{
    int err = -1;
    int detachstate;
  
    if (!ctx)
        return err;

    if(!routine)
        return err = -EINVAL;

    err = pthread_create(&ctx->handle,&ctx->attr,routine,user);

    pthread_attr_getdetachstate(&ctx->attr,&detachstate);
    
    if (detachstate == PTHREAD_CREATE_JOINABLE)
        pthread_attr_destroy(&ctx->attr);

    return err;
}

int cw_thread_create2(cw_thread_t *ctx,int joinable,void *(*routine)(void *user),void *user)
{
    int err = -1;
  
    if (!ctx)
        return err;

    if(!routine)
        return err = -EINVAL;

    pthread_attr_init(&ctx->attr);
    pthread_attr_setdetachstate(&ctx->attr,(joinable?PTHREAD_CREATE_JOINABLE:PTHREAD_CREATE_DETACHED));

    err = cw_thread_create(ctx,routine,user);

    return err;
}

int cw_thread_join(cw_thread_t *ctx)
{
    int err = -1;
    int detachstate = -1;

    if (!ctx)
        return err;

    err = pthread_attr_getdetachstate(&ctx->attr,&detachstate);
    if (err != 0)
        return err;

    if (detachstate == PTHREAD_CREATE_JOINABLE)
    {
        err = pthread_join(ctx->handle, &ctx->result);

        pthread_attr_destroy(&ctx->attr);
    }

    return err;
}

int cw_thread_setname(const char* fmt,...)
{
    int err = -1;
    char name[16] = {0};

    if(!fmt || !fmt[0])
        return err;

    va_list vaptr;
    va_start(vaptr, fmt);
    snprintf(name,16,fmt,vaptr);
    va_end(vaptr);

    err = pthread_setname_np(pthread_self(),name);

    return err;
}

int cw_thread_getname(char name[16])
{
    int err = -1;

    if(!name)
        return err;

    err = pthread_getname_np(pthread_self(),name,16);

    return err; 
}