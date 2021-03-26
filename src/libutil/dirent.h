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
 * 拼接目录。
 * 
 * 自动检查前后的'/'字符，接拼位置只保留一个'/'字符，或自动添加一个'/'字符。
 * 
 * @warning 要有足够的可用空间，不然会溢出。
*/
char *good_dirdir(char *path,const char *suffix);

/**
 * 创建目录。
 * 
 * 支持创建多级目录。如果末尾不是'/'，则最后一级的名称会被当做文件名而忽略。
*/
void good_mkdir(const char *path,mode_t mode);

/**
 * 截取目录。
 * 
 * 最后一级的名称会被裁剪，并且无论目录结构是否真存在都会截取。 
 *
 * @see dirname()
*/
char *good_dirname(char *dst, const char *src);

/**
 * 截取目录或文件名称。
 * 
 * 最后一级的名称'/'(包括)之前的会被裁剪，并且无论目录结构是否真存在都会截取。 
 * 
 * @see basename()
*/
char *good_basename(char *dst, const char *src);

/**
 * 美化目录。
 * 
 * 不会检测目录结构是否存在。
 * 
 * 例：/aaaa/bbbb/../ccc -> /aaaa/ccc
 * 例：/aaaa/bbbb/./ccc -> /aaaa/bbbb/ccc
*/
char *good_dirnice(char *dst, const char *src);

/**
 * 目录扫描。
 * 
 * 扫描的结果会自动生成一个颗“树”。
 * 
 * @param deep 遍历深度。0，1，是等效的。
 * @param onefs 0 不辨别文件系统是否相同，!0 只在同一个文件系统中遍历。
 * 
 * @note 如果目录和文件较多，则需要较多的内存。
 * 
 * @see good_tree_scan()
 * @see good_tree_fprintf()
 * @see good_tree_vfprintf()
 * 
*/
void good_dirscan(good_tree_t *tree, const char *path, size_t deep,int onefs);


#endif //GOOD_UTIL_DIRENT_H