/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_OPTION_H
#define GOOD_UTIL_OPTION_H

#include "general.h"
#include "tree.h"

/**
 * 选项的字段。
*/
enum _good_option_field
{
    /**
     * Key 字段索引。
    */
   GOOD_OPTION_KEY = 0,
#define GOOD_OPTION_KEY     GOOD_OPTION_KEY

    /**
     * Value 字段索引。
    */
   GOOD_OPTION_VALUE = 0
#define GOOD_OPTION_VALUE   GOOD_OPTION_VALUE

};

/**
 * 配置一个选项。
 * 
 * @param value 值的指针，可以为NULL0)。
 * 
 * 支持一对多键值组合，相同键的值次序由添加顺序决定。
*/
int good_option_set(good_tree_t *opt, const char *key, const char *value);

/**
 * 获取一个选项的值。
 * 
 * @param defval 默认值，可以为NULL(0)。
 * 
 * @return !NULL(0) 成功(值的指针)， NULL(0) 失败(键不存在)。
*/
const char* good_option_get(good_tree_t *opt, const char *key,size_t index,const char* defval);

/**
 * 统计选项值的数量。
 * 
 * @param defval 默认值，可以为NULL(0)。
 * 
 * @return >=0 成功(值的数量)，< 0 失败(键不存在)。
*/
ssize_t good_option_count(good_tree_t *opt, const char *key);

/**
 * 格式化打印。
 * 
 * @return >=0 输出的长度，< 0 失败。
*/
ssize_t good_option_fprintf(FILE *fp,good_tree_t *opt);

/**
 * 格式化打印。
 * 
 * @return >=0 输出的长度，< 0 失败。
*/
ssize_t good_option_snprintf(char* buf,size_t max,good_tree_t *opt);

#endif //GOOD_UTIL_OPTION_H