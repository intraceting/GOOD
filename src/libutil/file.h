/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_FILE_H
#define GOOD_UTIL_FILE_H

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

/**
 * 路径拼接
 * 
 * @warning 要有足够的可用空间，不然会溢出。
*/
char *good_path_concat(char *path,const char *suffix);

/**
 * 路径创建
 * 
*/
void good_path_create(const char *path,mode_t mode);

/**
 * 截取路径
 * 
*/
char *good_path_dirname(char *dst, const char *src);

/**
 * 截取文件名
 * 
*/
char *good_path_basename(char *dst, const char *src);




#endif //GOOD_UTIL_FILE_H