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
#include <stdarg.h>
#include <pthread.h>

/**
 * 
*/
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
 * 
 * @note 多线程共享KEY，但VALUE是线程私有。
*/
typedef struct _cw_specific
{
    /**
     * 私有数据状态
     * 
    */
    atomic_int status;
    /**
     * KEY未创建或出错。
    */
#define CW_SPECIFIC_STATUS_UNKNOWN      0
    /**
     * KEY创建成功。
     */
#define CW_SPECIFIC_STATUS_STABLE       1
    /**
     * KEY正在创建。
     */
#define CW_SPECIFIC_STATUS_SYNCHING     2

    /**
     * 私有数据KEY
     * 
    */
    pthread_key_t key;

    /**
     * 私有数据大小
     * 
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
 * 线程句柄和返回值
 * 
 * 
*/
typedef struct _cw_thread_t
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
 * 初始化互斥量
 * 
 * @see pthread_cond_init()
 * @see pthread_mutex_init()
 * 
 * @note 在调用此函数前需先初始化属性。
*/
void cw_mutex_init(cw_mutex_t *ctx);

/**
 * 初始化互斥量及属性
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
 * @see cw_mutex_init()
*/
void cw_mutex_init2(cw_mutex_t *ctx, int shared);

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
 * @param broadcast 0 通知一个等待线程，!0 通知所有等待线程。
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
*/
void cw_specific_default_free(void* m);

/**
 * 创建线程
 * 
 * @param joinable 0 线程结束后自回收资源，!0 线程结束后需要调用者回收资源。
 * @param routine 线程函数
 * @param user 线程用户指针
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_attr_init()
 * @see pthread_attr_setdetachstate()
 * @see pthread_attr_destroy()
 * @see pthread_create()
 * 
*/
int cw_thread_create(cw_thread_t *ctx,int joinable,void *(*routine)(void *user),void *user);

/**
 * 创建线程
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see cw_thread_create()
 * 
*/
int cw_thread_create2(cw_thread_t *ctx,void *(*routine)(void *user),void *user);

/**
 * 等待线程结束并回收资源
 * 
 * @return 0 成功；!0 出错。
 * 
 * @see pthread_attr_getdetachstate()
 * @see pthread_join()
 * 
 * @note 当线程被已经分离或已经分离模式创建的，直接返回。
*/
int cw_thread_join(cw_thread_t* ctx);

/**
 * 设置当前线程名字
 * 
 * @see pthread_setname_np()
 * 
 * @note 最大支持16个字节(Bytes)。
 * 
*/
int cw_thread_setname(const char* fmt,...);

/**
 * 获取当前线程名字
 * 
 * @see pthread_getname_np()
 * 
*/
int cw_thread_getname(char name[16]);


/**
 * 
*/
__END_DECLS



#endif // CWHEEL_THREAD_H