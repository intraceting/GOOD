/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_POPEN_H
#define GOOD_UTIL_POPEN_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "general.h"
#include "heap.h"
#include "buffer.h"
#include "file.h"
#include "map.h"


/**
 * 创建子进程，用于执行shell。
 *
 * @param env 环境变量。GOOD_MAP_KEY is name,GOOD_MAP_VALUE is value。NULL(0) 忽略。
 * 
 * @return 子进程ID 成功，-1 失败。
*/
pid_t good_popen(const char* cmd,int* stdin_fd, int* stdout_fd, int* stderr_fd,const good_map_t *env);

#endif //GOOD_UTIL_POPEN_H