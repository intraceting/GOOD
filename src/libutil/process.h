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
 * 单实例模式运行。
 * 
 * 文件句柄在退出前不要关闭，否则会使文件解除锁定状态。
 * 
 * 进程ID以十进制文本格式写入文件，例：2021 。
 * 
 * @param pid 当接口返回时，被赋值正在运行的进程ID。NULL(0) 忽略。
 * 
 * @return >= 0 文件句柄(当前进程是唯一进程)，-1 已有实例正在运行。
 * 
 * @see flock()
 * @see getpid()
*/
int good_proc_singleton(const char* lockfile,int* pid);



#endif //GOOD_UTIL_PROCESS_H