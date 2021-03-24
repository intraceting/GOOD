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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "general.h"
#include "buffer.h"
#include "tree.h"
#include "string.h"


/**
 * 拼接目录
 * 
 * @warning 要有足够的可用空间，不然会溢出。
*/
char *good_dirdir(char *path,const char *suffix);

/**
 * 创建目录
 * 
 * @note 末尾如果不是'/'，则最后一级名称会被当做文件名而忽略。
*/
void good_mkdir(const char *path,mode_t mode);

/**
 * 截取目录
 * 
*/
char *good_dirname(char *dst, const char *src);

/**
 * 截取文件名
 * 
*/
char *good_basename(char *dst, const char *src);

/**
 * 美化目录
 * 
 * @note 目录不存在也可以。
*/
char *good_dirnice(char *dst, const char *src);

/**
 * 目录扫描
 * 
 * @param deep 遍历深度。0，1，是等效的。
 * @param onefs 0 不辨别文件系统是否相同，!0 只在同一个文件系统中遍历。
 * 
 * @note 如果目录和文件较多，则需要较多的内存。
 * 
*/
void good_dirscan(good_tree_t *tree, const char *path, size_t deep,int onefs);


#endif //GOOD_UTIL_DIRENT_H