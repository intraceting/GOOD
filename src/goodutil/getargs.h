/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_GETARGS_H
#define GOOD_UTIL_GETARGS_H

#include "general.h"
#include "option.h"

/**
 * 导入参数。
 * 
 * 未关联键的值，使用前缀做为键。
 * 
 * @param prefix 键的前缀字符串的指针。
*/
void good_getargs(good_tree_t *opt, int argc, char *argv[],
                  const char *prefix);

/**
 * 从已经打开的文件导入参数。
 * 
 * 注释行将被忽略。
 * 
 * @param delim 分割字符。
 * @param note 注释字符。
 * @param argv0 命令字符串的指针，可以为NULL(0)。
 * 
*/
void good_getargs_fp(good_tree_t *opt, FILE *fp, uint8_t delim, char note,
                     const char *argv0, const char *prefix);

/**
 * 从文件导入参数。
 * 
 * 注释行将被忽略。
 * 
 * @param file 文件名(或带路径的文件名)的指针。
 * 
*/
void good_getargs_file(good_tree_t *opt, const char *file, uint8_t delim, char note,
                       const char *argv0, const char *prefix);

/**
 * 从文本导入参数。
 * 
 * @param text 文本的指针。
 * @param len 文本的长度。
*/
void good_getargs_text(good_tree_t *opt, const char *text, size_t len, uint8_t delim, char note,
                       const char *argv0, const char *prefix);

#endif //GOOD_UTIL_GETARGS_H