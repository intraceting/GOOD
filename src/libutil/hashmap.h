/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */

#ifndef GOOD_UTIL_HASHMAP_H
#define GOOD_UTIL_HASHMAP_H

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
typedef struct _good_hashmap_node
{
    /**
     * Key size
    */
    size_t ksize;

    /**
     * Key 
    */
    void* key;
    
    /**
     * Value size
    */
    size_t vsize;

    /**
     * Value 
    */
    void* value;

}good_hashmap_node_t;

/**
 * 
*/
typedef struct _good_hashmap
{
    /**
     * Key size
     * 
     * @note 0 Key是以'\0'为结束符的字符串。
    */
    size_t ksize;

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

}good_hashmap_t;

/**
 * BKDR32
 * 
 */
uint32_t good_hashmap_hash_bkdr(const void* data,size_t size);

/**
 * BKDR64
 * 
 */
uint64_t good_hashmap_hash_bkdr64(const void* data,size_t size);

/**
 * 
*/
good_tree_t* good_hashmap_lookup(good_tree_t* map,const void* key,int create);


#endif //GOOD_UTIL_HASHMAP_H