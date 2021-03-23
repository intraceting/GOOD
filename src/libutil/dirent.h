/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_DIRENT_H
#define GOOD_UTIL_DIRENT_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "general.h"
#include "heap.h"
#include "string.h"

/**
 * 拼接目录
 * 
 * @warning 要有足够的可用空间，不然会溢出。
*/
char *good_dircat(char *path,const char *suffix);

/**
 * 创建目录
 * 
 * @note 末尾如果不是'/'，则最后一级名称会被当做文件名而忽略。
*/
void good_mkdir(const char *path,mode_t mode);

/**
 * 截取路径
 * 
*/
char *good_dirname(char *dst, const char *src);

/**
 * 截取文件名
 * 
*/
char *good_basename(char *dst, const char *src);



#endif //GOOD_UTIL_DIRENT_H