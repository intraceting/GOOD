/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_SIGNAL_H
#define GOOD_UTIL_SIGNAL_H

#include "general.h"
#include "thread.h"

/**
 * 简单的信号处理。
*/
typedef struct _good_signal
{
    /** 
     * 信号集合。
    */
    sigset_t signals;
    
    /**
     * 处理函数，NULL(0)忽略信号。
     * 
     * @return -1 终止，1 继续。
    */
    int (*signal_cb)(const siginfo_t *info, void *opaque);

    /**
     * 环境指针。
    */
    void *opaque;
}good_signal_t;

/**
 * 等待信号。
 * 
 * @param timeout 超时(毫秒)。 >= 0 有信号或时间过期，< 0 直到有信号或出错。
*/
void good_sigwaitinfo(good_signal_t *sig, time_t timeout);

#endif //GOOD_UTIL_SIGNAL_H