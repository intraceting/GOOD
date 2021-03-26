/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_USER_H
#define GOOD_UTIL_USER_H

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
 * /var/run/user/$UID/
 * 
 * @param append 拼接目录或文件名。NULL(0) 忽略。
 * 
 * @see snprintf()
*/
char* good_user_dirname(char* buf,const char* append);

#endif //GOOD_UTIL_USER_H