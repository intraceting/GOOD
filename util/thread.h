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
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>

/**
 * 
*/
__BEGIN_DECLS


/**
 * 互斥量、事件
 * 
 * 
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
*/
int cw_mutex_lock(cw_mutex_t *ctx,int try);

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
 * 
*/
__END_DECLS

#endif // CWHEEL_THREAD_H