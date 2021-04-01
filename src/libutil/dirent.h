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