/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */

#ifndef GOOD_UTIL_VECTOR_H
#define GOOD_UTIL_VECTOR_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "general.h"
#include "buffer.h"
#include "tree.h"

/**
 * 
*/
typedef struct _good_vector
{
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

}good_vector_t;

/**
 * HASH
 * 
 * @see good_hash_bkdr64()
*/
uint64_t good_vector_default_hash(const void* data,size_t size);

/**
 * 比较
 * 
 * @see memcmp()
*/
int good_vector_default_compare(const void *data1, const void *data2, size_t size);

/**
 * 销毁
*/
void good_vector_destroy(good_vector_t* vec);

/**
 * 初始化
 * 
 * @return 0 成功，!0 失败。
*/
int good_vector_init(good_vector_t* vec,size_t size);

/*
 *
*/
good_tree_t* good_vector_seek(good_vector_t* vec,size_t index);

/*
 *
*/
good_tree_t* good_vector_find(good_vector_t* vec,const void* key,size_t ksize,int create);

/*
 *
*/
void good_vector_erase(good_vector_t* vec,const void* key,size_t ksize);



#endif //GOOD_UTIL_VECTOR_H