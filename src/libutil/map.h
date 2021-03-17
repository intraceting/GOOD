/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_MAP_H
#define GOOD_UTIL_MAP_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "general.h"
#include "buffer.h"
#include "tree.h"
#include "hash.h"

/**
 * 
*/
typedef struct _good_map
{
    /**
     * 根 
     * 
     * @note 尽量不要直接修改。
    */
    good_tree_t root;

    /**
     * 大小
     * 
     * @note 尽量不要直接修改。
    */
    size_t size;

    /**
     * 表
     * 
     * @see good_tree_t
     * 
     * @note 尽量不要直接修改。
    */
    good_tree_t **table;

    /**
     * HASH
     * 
     * @see good_vector_hash()
    */
    uint64_t (*hash_cb)(const void* key,size_t size);

    /**
     * 比较
     * 
     * @see good_vector_compare()
    */
    int (*compare_cb)(const void *key1, const void *key2, size_t size);

}good_map_t;

/**
 * HASH
 * 
 * @see good_hash_bkdr64()
*/
uint64_t good_map_hash(const void* data,size_t size);

/**
 * 比较
 * 
 * @see memcmp()
*/
int good_map_compare(const void *data1, const void *data2, size_t size);

/**
 * 销毁
*/
void good_map_destroy(good_map_t* map);

/**
 * 初始化
 * 
 * @return 0 成功，!0 失败。
*/
int good_map_init(good_map_t* map,size_t size);




#endif //GOOD_UTIL_MAP_H