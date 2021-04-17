/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_OPTION_H
#define GOOD_UTIL_OPTION_H

#include "general.h"
#include "map.h"
#include "vector.h"

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
     * @warning 尽量不要直接修改。
     * 
     * @see good_map_t
     * 
    */
    good_map_t table;

} good_option_t;

void good_option_destroy(good_option_t *opt);

int good_option_init(good_option_t *opt);

int good_option_set(good_option_t *opt, const char *key, const char *value);

const char* good_option_get(const good_option_t *opt, const char *key,int index);

#endif //GOOD_UTIL_OPTION_H