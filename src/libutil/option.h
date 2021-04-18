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
 * 选项。
 * 
 * 支持一对多键值组合。
*/
typedef struct _good_option
{
    /**
     * 表格。
     * 
     * @note 尽量不要直接修改。
     * 
     * @see good_tree_scan()
     * @see good_tree_fprintf()
     * @see good_tree_vfprintf()
    */
    good_tree_t *table;

    /**
     * KEY比较函数。
     * 
     * @see good_map_compare()
    */
    int (*compare_cb)(const void *key1, const void *key2,void *opaque);

} good_option_t;

/**
 * 比较函数。
 * 
 * @return > 0 is data1 > data2，0 is data1 == data2，< 0 is data1 < data2。
 * 
 * @see memcmp()
*/
int good_option_compare(const void *data1, const void *data2,void *opaque);

void good_option_destroy(good_option_t *opt);

int good_option_init(good_option_t *opt);

int good_option_set(good_option_t *opt, const char *key, const char *value);

const char* good_option_get(const good_option_t *opt, const char *key,size_t index,const char* defval);

#endif //GOOD_UTIL_OPTION_H