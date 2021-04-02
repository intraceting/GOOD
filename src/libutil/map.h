/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_MAP_H
#define GOOD_UTIL_MAP_H

#include "general.h"
#include "tree.h"

/**
 * MAP。
 * 
 * HASH和DEQUE存储结构。
*/
typedef struct _good_map
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
     * HASH函数。
     * 
     * @see good_map_hash()
    */
    uint64_t (*hash_cb)(const void* key,size_t size,void *opaque);

    /**
     * 比较函数。
     * 
     * @see good_map_compare()
    */
    int (*compare_cb)(const void *key1, const void *key2, size_t size,void *opaque);

    /**
     * 销毁函数。
    */
    void (*destroy_cb)(good_allocator_t *alloc, void *opaque);

    /**
    * 环境指针。
    */
    void *opaque;
    
}good_map_t;

/**
 * MAP索引
*/
enum _good_map_index
{
    /**
     * Bucket 索引
    */
   GOOD_MAP_BUCKET = 0,
#define GOOD_MAP_BUCKET     GOOD_MAP_BUCKET

    /**
     * Key 索引
    */
   GOOD_MAP_KEY = 0,
#define GOOD_MAP_KEY        GOOD_MAP_KEY

    /**
     * Value 索引
    */
   GOOD_MAP_VALUE = 1
#define GOOD_MAP_VALUE      GOOD_MAP_VALUE
};

/**
 * HASH函数。
 * 
 * @see good_hash_bkdr64()
*/
uint64_t good_map_hash(const void* data,size_t size,void *opaque);

/**
 * 比较函数。
 * 
 * @return > 0 is key1 > key2，0 is key1 == key2，< 0 is key1 < key2。
 * 
 * @see memcmp()
*/
int good_map_compare(const void *data1, const void *data2, size_t size,void *opaque);

/**
 * 销毁。
*/
void good_map_destroy(good_map_t* map);

/**
 * 初始化。
 * 
 * @return 0 成功，!0 失败。
*/
int good_map_init(good_map_t* map,size_t size);

/**
 * 查找或创建。
 * 
 * @param ksize Key size
 * @param vsize Value size。 0 仅查找，>0 不存在则创建。
*/
good_tree_t* good_map_find(good_map_t* map,const void* key,size_t ksize,size_t vsize);

/**
 * 擦除。
 * 
 * @see good_map_find()
*/
void good_map_erase(good_map_t* map,const void* key,size_t ksize);


#endif //GOOD_UTIL_MAP_H