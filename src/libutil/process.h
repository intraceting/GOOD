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

#include "general.h"
#include "heap.h"
#include "dirent.h"
#include "file.h"
#include "string.h"


/**
 * 获取当前用户的运行路径。
 * 
 * @param append 拼接目录或文件名。NULL(0) 忽略。
 * 
 * @note /var/run/user/$UID/
 * 
 * @warning 运行路径可能不存在。
 * 
*/
char* good_user_dirname(char* buf,const char* append);

/**
 * 获取当前程序的完整路径和文件名。
 * 
*/
char* good_app_pathfile(char* buf);

/**
 * 获取当前程序的完整路径。
 * 
 * @param append 拼接目录或文件名。NULL(0) 忽略。
 * 
*/
char* good_app_dirname(char* buf,const char* append);

/**
 * 获取当前程序的文件名。
 * 
*/
char* good_app_basename(char* buf);

/**
 * 单实例模式运行。
 * 
 * @param pid 正在运行的进程ID。NULL(0) 忽略。
 * 
 * @return >= 0 锁定文件句柄，-1 已有实例正在运行。
 * 
 * @note $PID 以10进制文本格式写入文件。例：2021
 * 
 * @warning 返回的句柄在退出前不要关闭，否则会使文件解除锁定状态。
*/
int good_app_singleton(const char* lockfile,int* pid);



#endif //GOOD_UTIL_PROCESS_H