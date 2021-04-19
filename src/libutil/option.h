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
 * 检测键是否存在。
 * 
 * @return !0 存在， 0 不存在。
*/
#define good_option_exist(opt, key) (good_option_count((opt), (key)) >= 0 ? 1 : 0)

/**
 * 删除一个选项和值。
 * 
 * @return 0 成功，-1 失败(键不存在)。
*/
int good_option_remove(good_tree_t *opt, const char *key);

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

/**
 * 解析参数选项。
 * 
 * @warning 未关联键的值，使用前缀做为键。
 * 
 * @param prefix 键的前缀字符串。
*/
void good_option_parse(good_tree_t *opt,int argc, char* argv[],const char *prefix);

/**
 * 导入参数选项。
 * 
 * @warning 注释行将被忽略。
 * 
 * @param note 注释字符，可以为'\0'。
 * @param argv0 命令字符串，可以为NULL(0)。
 * @param prefix 键的前缀字符串。
*/
void good_option_fimport(good_tree_t *opt,FILE *fp,char note,const char *argv0,const char *prefix);

/**
 * 导入参数选项。
 * 
 * @warning 注释行将被忽略。
 * 
 * @param note 注释字符，可以为'\0'。
 * @param argv0 命令字符串，可以为NULL(0)。
 * @param prefix 键的前缀字符串。
*/
void good_option_import(good_tree_t *opt,const char *name,char note,const char *argv0,const char *prefix);

/**
 * 导入参数选项。
 * 
 * @warning 注释行将被忽略。
 * 
 * @param note 注释字符，可以为'\0'。
 * @param argv0 命令字符串，可以为NULL(0)。
 * @param prefix 键的前缀字符串。
*/
void good_option_memimport(good_tree_t *opt,const char *text,size_t len,char note,const char *argv0,const char *prefix);


#endif //GOOD_UTIL_OPTION_H