/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_PROCESS_H
#define GOOD_UTIL_PROCESS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/file.h>

#include "general.h"
#include "heap.h"
#include "dirent.h"
#include "file.h"
#include "string.h"


/**
 * 获取当前用户的运行路径。
 * 
 * 可能不存在，使用前最好检查一下。
 * 
 * @param append 拼接目录或文件名。NULL(0) 忽略。
 * 
 * @note /var/run/user/$UID/
 * 
*/
char* good_user_dirname(char* buf,const char* append);

/**
 * 获取当前程序的完整路径和文件名。
 * 
 * @see readlink()
*/
char* good_app_pathfile(char* buf);

/**
 * 获取当前程序的完整路径。
 * 
 * @param append 拼接目录或文件名。NULL(0) 忽略。
 * 
 * @see good_app_pathfile()
 * @see good_dirname()
*/
char* good_app_dirname(char* buf,const char* append);

/**
 * 获取当前程序的文件名。
 * 
 * @see good_app_pathfile()
 * @see good_basename()
*/
char* good_app_basename(char* buf);

/**
 * 单实例模式运行。
 * 
 * 返回的句柄在退出前不要关闭，否则会使文件解除锁定状态。
 * 
 * @param pid 正在运行的进程ID。NULL(0) 忽略。
 * 
 * @return >= 0 锁定文件句柄，-1 已有实例正在运行。
 * 
 * @note $PID 以10进制文本格式写入文件。例：2021
 * 
 * @see good_open()
 * @see flock()
 * @see good_closep()
*/
int good_run_singleton(const char* lockfile,int* pid);

/**
 * 等待信号。
 * 
 * @param timeout >= 0 当信号到达或时间(毫秒)过期即返回，< 0 直到信号到达或出错返回。
 * @param signal_cb 信号处理函数。NULL(0) 忽略信号。返回 !0 继续，0 终止。
 * 
 * @return >=0 成功，< 0 失败或超时。
 * 
*/
int good_wait_signal(sigset_t *sig, time_t timeout,
                     int (*signal_cb)(const siginfo_t *info, void *opaque), 
                     void *opaque);

#endif //GOOD_UTIL_PROCESS_H