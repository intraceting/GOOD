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
 * 等待信号。
 * 
 * @param timeout 超时(毫秒)。 >= 0 有信号或时间过期，< 0 直到有信号或出错。
 * @param signal_cb 信号处理函数，可以为NULL(0)。!0 继续，0 终止。
 * 
 * @return >=0 成功，< 0 失败或超时。
 * 
*/
int good_sigwaitinfo(sigset_t *sig, time_t timeout,
                     int (*signal_cb)(const siginfo_t *info, void *opaque),
                     void *opaque);

#endif //GOOD_UTIL_SIGNAL_H