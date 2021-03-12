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

#include "macro.h"
#include "buffer.h"
#include "tree.h"

/**
 * 
*/
typedef struct _good_map_node
{
    /**
     * @see good_tree_t
     * 
     * @note 必须是第一个元素。
     * @note 尽量不要直接访问或修改。
    */
    good_tree_t key;
   
    /**
     * Value size
    */
    size_t vsize;

    /**
     * Value 
    */
    void* value;

}good_map_node;

/**
 * 
*/
typedef struct _good_map
{
    /**
     * @see good_tree_t
    */
    good_tree_t root;

    /**
     * HASH
     * 
     * 
    */
    uint32_t (*hash_cb)(const void* key,size_t size);

    /**
     * 
     * 比较
     * 
     * @return -1 is key1 < key2, 0 is key1 == key2, 1  is key1 > key2.
    */
    int (*compare_cb)(const void *key1, const void *key2, size_t size);

}good_map_t;

/**
 * BKDR32
 * 
 */
uint32_t good_map_hash_bkdr(const void* data,size_t size);

/**
 * BKDR64
 * 
 */
uint64_t good_map_hash_bkdr64(const void* data,size_t size);

/**
 * 比较
 * 
 * @see memcmp()
*/
int good_map_compare(const void *data1, const void *data2, size_t size);


#endif //GOOD_UTIL_HASHMAP_H