/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKUTIL_THREAD_H
#define ABCDKUTIL_THREAD_H

#include "general.h"

__BEGIN_DECLS

/*------------------------------------------------------------------------------------------------*/

/**
 * 互斥量、事件。
 * 
 * 如果需要支持跨进程特性，需要让结构体数据运行在共享内存中。
*/
typedef struct _abcdk_mutex
{
    /**
     * 事件属性。
    */
    pthread_condattr_t condattr;

    /**
     * 事件。
    */
    pthread_cond_t cond;

    /**
     * 互斥量属性。
    */
    pthread_mutexattr_t mutexattr;

    /**
     * 互斥量。
    */
    pthread_mutex_t mutex;

} abcdk_mutex_t;

/*------------------------------------------------------------------------------------------------*/

/**
 * 线程句柄和返回值。
 * 
*/
typedef struct _abcdk_thread_t
{
    /**
     * 句柄。
     * 
    */
    pthread_t handle;

    /**
     * 返回值。
     * 
    */
    void* result;

    /**
     * 线程函数。
    */
    void *(*routine)(void *opaque);

    /**
     * 环境指针。
     */
    void *opaque;

} abcdk_thread_t;

/*------------------------------------------------------------------------------------------------*/

/**
 * 销毁互斥量及属性。
*/
void abcdk_mutex_destroy(abcdk_mutex_t *ctx);

/**
 * 初始化互斥量。
 * 
 * 在调用此函数前需先初始化属性。
*/
void abcdk_mutex_init(abcdk_mutex_t *ctx);

/**
 * 初始化互斥量及属性。
 * 
 * 当互斥量拥用共享属性时，在多进程间有效。
 * 
 * @param shared 0 私有，!0 共享。
*/
void abcdk_mutex_init2(abcdk_mutex_t *ctx, int shared);

/**
 * 互斥量加锁。
 * 
 * @param nonblock 0 直到成功或出错返回，!0 尝试一下即返回。
 * 
 * @return 0 成功，!0 出错。
 * 
*/
int abcdk_mutex_lock(abcdk_mutex_t *ctx, int nonblock);

/**
 * 互斥量解锁。
 * 
 * @return 0 成功；!0 出错。
*/
int abcdk_mutex_unlock(abcdk_mutex_t *ctx);

/**
 * 等待事件通知。
 * 
 * @param timeout 超时(毫秒)。>= 0 有事件或时间过期，< 0 直到有事件或出错。
 * 
 * @return 0 成功；!0 出错(errno)。
*/
int abcdk_mutex_wait(abcdk_mutex_t *ctx, time_t timeout);

/**
 * 发出事件通知。
 * 
 * @param broadcast 是否广播事件通知。0 否，!0 是。
 * 
 * @return 0 成功；!0 出错。
*/
int abcdk_mutex_signal(abcdk_mutex_t *ctx, int broadcast);

/*------------------------------------------------------------------------------------------------*/

/**
 * 创建线程。
 * 
 * @param joinable 0 结束后自回收资源，!0 结束后需要调用者回收资源。
 * 
 * @return 0 成功；!0 出错。
 * 
*/
int abcdk_thread_create(abcdk_thread_t *ctx,int joinable);

/**
 * 等待线程结束并回收资源。
 * 
 * 当线程被已经分离或已经分离模式创建的，直接返回。
 * 
 * @return 0 成功；!0 出错。
*/
int abcdk_thread_join(abcdk_thread_t* ctx);

/*------------------------------------------------------------------------------------------------*/

/**
 * 设置当前线程名字。
 * 
 * 最大支持16个字节(Bytes)。
 * 
 * @return 0 成功；!0 出错。
 * 
*/
int abcdk_thread_setname(const char* fmt,...);

/**
 * 获取当前线程名字。
 * 
 * @return 0 成功；!0 出错。
 * 
*/
int abcdk_thread_getname(char name[16]);

/*------------------------------------------------------------------------------------------------*/

/**
 * 选举主线程
 * 
 * @return 0 成功；!0 出错。
 * 
 */
int abcdk_thread_leader_test(volatile pthread_t *tid);

/**
 * 主线程退出
 *
 *  @return 0 成功；!0 出错。
 * 
 */
int abcdk_thread_leader_quit(volatile pthread_t *tid);

/*------------------------------------------------------------------------------------------------*/

__END_DECLS

#endif // ABCDKUTIL_THREAD_H