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
#include <errno.h>

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
     * 表格 
     * 
     * @note 尽量不要直接修改。
    */
    good_tree_t *table;

    /**
     * Index 字段索引
    */
#define GOOD_MAP_INDEX  0

    /**
     * Key 字段索引
    */
#define GOOD_MAP_KEY    0

    /**
     * Value 字段索引
    */
#define GOOD_MAP_VALUE  1

    /**
     * HASH
     * 
     * @see good_map_hash()
    */
    uint64_t (*hash_cb)(const void* key,size_t size,void *opaque);

    /**
     * 比较
     * 
     * @see good_map_compare()
    */
    int (*compare_cb)(const void *key1, const void *key2, size_t size,void *opaque);

    /**
     * 释放
     * 
     * @note NULL(0) 忽略。
    */
    void (*free_cb)(uint8_t **data,size_t number,void *opaque);

    /**
     * 私有指针
     * 
    */
    void *opaque;

}good_map_t;

/**
 * HASH
 * 
 * @see good_hash_bkdr64()
*/
uint64_t good_map_hash(const void* data,size_t size,void *opaque);

/**
 * 比较
 * 
 * @see memcmp()
*/
int good_map_compare(const void *data1, const void *data2, size_t size,void *opaque);

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

/**
 * 查找或创建
 * 
 * @param ksize Key size
 * @param vsize Value size。 0 仅查找，>0 不存在则创建。
*/
good_tree_t* good_map_find(good_map_t* map,const void* key,size_t ksize,size_t vsize);

/**
 * 擦除
*/
void good_map_erase(good_map_t* map,const void* key,size_t ksize);





#endif //GOOD_UTIL_MAP_H