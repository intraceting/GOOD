/*
 * This file is part of CWheel.
 * 
 * MIT License
 * 
 */
#ifndef CWHEEL_THREAD_H
#define CWHEEL_THREAD_H

/**/
#include <errno.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

/**/
__BEGIN_DECLS

/**
 * 互斥量、事件
 * 
 * @note 如果需要支持跨进程特性，需要让结构体数据运行在共享内存中。
*/
typedef struct _cw_mutex
{
    /**
     * 事件属性
    */
    pthread_condattr_t condattr;
    /**
     * 事件
    */
    pthread_cond_t cond;
    /**
     * 互斥量属性
    */
    pthread_mutexattr_t mutexattr;
    /**
     * 互斥量
    */
    pthread_mutex_t mutex;

} cw_mutex_t;

/**
 * 私有数据
*/
typedef struct _cw_specific
{
    /**
     * 私有数据状态
     * 
    */
    atomic_int status;
#define CW_SPECIFIC_STATUS_UNKNOWN      0 /*未知的*/
#define CW_SPECIFIC_STATUS_STABLE       1 /*稳定的*/
#define CW_SPECIFIC_STATUS_SYNCHING     2 /*同步中*/

    /**
     * 私有数据KEY
     * 
     * @see pthread_key_create()
    */
    pthread_key_t key;
    /**
     * 私有数据大小
    */
    size_t size;
    /**
     * 申请私有数据
     * 
     * @see cw_specific_default_alloc()
     * 
    */
    void *(*alloc_cb)(size_t s);
    /**
     * 释放私有数据
     * 
     * @see cw_specific_default_free()
     * 
     * @note 如果私有数据无法自动释放，需要自定义此回调函数。
    */
    void (*free_cb)(void* m);
} cw_specific_t;

/**
 * 
*/
typedef struct _cw_thread_t
{
    /**
     * 
    */
    pthread_attr_t attr;
    /**
     * 
    */
    pthread_t handle;
    /**
     * 
    */
    void* result;
    /**
     * 
    */
    void *(*routine_cb)(void *user);
    /**
     * 
    */
    void *user;

} cw_thread_t;

/**
 * 销毁互斥量及属性
 * 
 * @see pthread_condattr_destroy()
 * @see pthread_cond_destroy()
 * @see pthread_mutexattr_destroy()
 * @see pthread_mutex_destroy()
*/
void cw_mutex_destroy(cw_mutex_t *ctx);

/**
 * 创建互斥量及属性
 * 
 * @param shared 0 私有，!0 共享。
 * 
 * @note 当互斥量拥用共享属性时，在多进程间有效。
 * 
 * @see pthread_condattr_init()
 * @see pthread_condattr_setclock()
 * @see pthread_condattr_setpshared()
 * @see pthread_cond_init()
 * @see pthread_mutexattr_init()
 * @see pthread_mutexattr_setpshared()
 * @see pthread_mutexattr_setrobust()
 * @see pthread_mutex_init()
*/
void cw_mutex_init(cw_mutex_t *ctx, int shared);

/**
 * 互斥量加锁
 * 
 * @param try 0 直到成功或出错返回，!0 尝试一下即返回。
 * 
 * @return 0 成功，!0 出错。
 * 
 * @see pthread_mutex_lock()
 * @see pthread_mutex_trylock()
 * @see pthread_mutex_consistent()
 * @see pthread_mutex_unlock()
*/
int cw_mutex_lock(cw_mutex_t *ctx, int try);

/**
 * 互斥量解锁
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_mutex_unlock()
*/
int cw_mutex_unlock(cw_mutex_t *ctx);

/**
 * 等待事件通知
 * 
 * @param timeout >= 0 当通知到达或时间(毫秒)过期即返回，< 0 直到通知到达或出错返回。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_cond_timedwait()
 * @see pthread_cond_wait()
*/
int cw_mutex_wait(cw_mutex_t *ctx, int64_t timeout);

/**
 * 发出事件通知
 * 
 * @param broadcast 0 通知一个等待线程，!0 通知所有的等待线程。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_cond_broadcast()
 * @see pthread_cond_signal()
*/
int cw_mutex_signal(cw_mutex_t *ctx, int broadcast);

/**
 * 销毁私有数据
 * 
 * @note 如果私有数据无法自动释放，则需要在线程退出前调用此函数。
 * 
 * @see pthread_key_delete()
 * @see pthread_setspecific()
 * @see pthread_getspecific()
*/
void cw_specific_destroy(cw_specific_t*ctx);

/**
 * 私有数据指针
 * 
 * @return NULL(0) 出错，!NULL(0) 指针。
 * 
 * @see pthread_key_create()
 * @see pthread_setspecific()
 * @see pthread_getspecific()
 * 
*/
void* cw_specific_value(cw_specific_t*ctx);

/**
 * 私有数据默认申请函数
 * 
 * @see malloc()
 * @see memset()
*/
void* cw_specific_default_alloc(size_t s);

/**
 * 私有数据默认释放函数
 * 
 * @see free()
 * 
 * @note 如果私有数据无法自动释放，则需要在线程退出前调用，否则会有内存泄漏问题。
*/
void cw_specific_default_free(void* m);

/**
 * 创建线程
 * 
 * @param joinable  0 线程结束后自动释放资源，!0 调用者负责释放资源。
 * 
 * @see pthread_attr_init()
 * @see pthread_attr_setdetachstate()
 * @see pthread_create()
 * @see pthread_attr_destroy();
*/
int cw_thread_create(cw_thread_t* ctx,int joinable);

/**
 * 等待线程结束
 * 
 * 
 * @see pthread_attr_getdetachstate()
 * @see pthread_join()
 * @see pthread_attr_destroy()
 * 
*/
int cw_thread_join(cw_thread_t* ctx);

/**/
__END_DECLS



#endif // CWHEEL_THREAD_H