/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_OPTION_H
#define ABTKUTIL_OPTION_H

#include "general.h"
#include "tree.h"

/**
 * 选项的字段。
*/
enum _abtk_option_field
{
    /**
     * Key 字段索引。
    */
   ABTK_OPTION_KEY = 0,
#define ABTK_OPTION_KEY     ABTK_OPTION_KEY

    /**
     * Value 字段索引。
    */
   ABTK_OPTION_VALUE = 0
#define ABTK_OPTION_VALUE   ABTK_OPTION_VALUE

};

/**
 * 配置一个选项。
 * 
 * @param value 值的指针，可以为NULL0)。
 * 
 * 支持一对多键值组合，相同键的值次序由添加顺序决定。
*/
int abtk_option_set(abtk_tree_t *opt, const char *key, const char *value);

/**
 * 获取一个选项的值。
 * 
 * @param defval 默认值，可以为NULL(0)。
 * 
 * @return !NULL(0) 成功(值的指针)， NULL(0) 失败(键不存在)。
*/
const char* abtk_option_get(abtk_tree_t *opt, const char *key,size_t index,const char* defval);

/**
 * 获取一个选项的值(整型)。
*/
int abtk_option_get_int(abtk_tree_t *opt, const char *key,size_t index,int defval);

/**
 * 获取一个选项的值(长整型)。
*/
long abtk_option_get_long(abtk_tree_t *opt, const char *key,size_t index,long defval);

/**
 * 获取一个选项的值(浮点型)。
*/
double abtk_option_get_double(abtk_tree_t *opt, const char *key,size_t index,double defval);

/**
 * 统计选项值的数量。
 * 
 * @param defval 默认值，可以为NULL(0)。
 * 
 * @return >=0 成功(值的数量)，< 0 失败(键不存在)。
*/
ssize_t abtk_option_count(abtk_tree_t *opt, const char *key);

/**
 * 检测键是否存在。
 * 
 * @return !0 存在， 0 不存在。
*/
#define abtk_option_exist(opt, key) (abtk_option_count((opt), (key)) >= 0 ? 1 : 0)

/**
 * 删除一个选项和值。
 * 
 * @return 0 成功，-1 失败(键不存在)。
*/
int abtk_option_remove(abtk_tree_t *opt, const char *key);

/**
 * 格式化打印。
 *  
 * @return >=0 输出的长度，< 0 失败。
*/
ssize_t abtk_option_fprintf(FILE *fp,abtk_tree_t *opt);

/**
 * 格式化打印。
 * 
 * @return >=0 输出的长度，< 0 失败。
*/
ssize_t abtk_option_snprintf(char* buf,size_t max,abtk_tree_t *opt);

#endif //ABTKUTIL_OPTION_H