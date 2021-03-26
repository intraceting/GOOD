/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_THREAD_H
#define GOOD_UTIL_THREAD_H

/**/
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

/**/
#include "general.h"
#include "buffer.h"

/**
 * 
*/
__BEGIN_DECLS

/**
 * 互斥量、事件
 * 
 * @note 如果需要支持跨进程特性，需要让结构体数据运行在共享内存中。
*/
typedef struct _good_mutex
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

} good_mutex_t;


/**
 * 线程句柄和返回值
 * 
 * 
*/
typedef struct _good_thread_t
{
    /**
     * 句柄
     * 
    */
    pthread_t handle;

    /**
     * 返回值
     * 
    */
    void* result;

    /**
     * @see pthared_create() 
     */
    void *(*routine)(void *opaque);

    /**
     * @see pthared_create() 
     */
    void *opaque;

} good_thread_t;

/**
 * 销毁互斥量及属性。
 * 
 * @see pthread_condattr_destroy()
 * @see pthread_cond_destroy()
 * @see pthread_mutexattr_destroy()
 * @see pthread_mutex_destroy()
*/
void good_mutex_destroy(good_mutex_t *ctx);

/**
 * 初始化互斥量。
 * 
 * 在调用此函数前需先初始化属性。
 * 
 * @see pthread_cond_init()
 * @see pthread_mutex_init()
*/
void good_mutex_init(good_mutex_t *ctx);

/**
 * 初始化互斥量及属性。
 * 
 * 当互斥量拥用共享属性时，在多进程间有效。
 * 
 * @param shared 0 私有，!0 共享。
 * 
 * @see pthread_condattr_init()
 * @see pthread_condattr_setclock()
 * @see pthread_condattr_setpshared()
 * @see pthread_cond_init()
 * @see pthread_mutexattr_init()
 * @see pthread_mutexattr_setpshared()
 * @see pthread_mutexattr_setrobust()
 * @see pthread_mutex_init()
 * @see good_mutex_init()
*/
void good_mutex_init2(good_mutex_t *ctx, int shared);

/**
 * 互斥量加锁。
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
int good_mutex_lock(good_mutex_t *ctx, int try);

/**
 * 互斥量解锁。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_mutex_unlock()
*/
int good_mutex_unlock(good_mutex_t *ctx);

/**
 * 等待事件通知。
 * 
 * @param timeout >= 0 当通知到达或时间(毫秒)过期即返回，< 0 直到通知到达或出错返回。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_cond_timedwait()
 * @see pthread_cond_wait()
*/
int good_mutex_wait(good_mutex_t *ctx, time_t timeout);

/**
 * 发出事件通知。
 * 
 * @param broadcast 0 通知一个等待线程，!0 通知所有等待线程。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_cond_broadcast()
 * @see pthread_cond_signal()
*/
int good_mutex_signal(good_mutex_t *ctx, int broadcast);

/**
 * 创建线程。
 * 
 * @param joinable 0 结束后自回收资源，!0 结束后需要调用者回收资源。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_attr_init()
 * @see pthread_attr_setdetachstate()
 * @see pthread_attr_destroy()
 * @see pthread_create()
 * 
*/
int good_thread_create(good_thread_t *ctx,int joinable);

/**
 * 等待线程结束并回收资源。
 * 
 * 当线程被已经分离或已经分离模式创建的，直接返回。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_attr_getdetachstate()
 * @see pthread_join()
*/
int good_thread_join(good_thread_t* ctx);

/**
 * 设置当前线程名字。
 * 
 * 最大支持16个字节(Bytes)。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_setname_np()
 * 
*/
int good_thread_setname(const char* fmt,...);

/**
 * 获取当前线程名字。
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_getname_np()
 * 
*/
int good_thread_getname(char name[16]);


/**
 * 
*/
__END_DECLS



#endif // GOOD_UTIL_THREAD_H