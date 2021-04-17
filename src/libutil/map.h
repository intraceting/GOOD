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
     * KEY哈希函数。
     * 
     * @see good_map_hash()
    */
    uint64_t (*hash_cb)(const void* key,size_t size,void *opaque);

    /**
     * KEY比较函数。
     * 
     * @see good_map_compare()
    */
    int (*compare_cb)(const void *key1, const void *key2, size_t size,void *opaque);

    /** 
     * 构造函数。
    */
    void (*construct_cb)(good_allocator_t *alloc, void *opaque);

    /**
     * 析构函数。
    */
    void (*destructor_cb)(good_allocator_t *alloc, void *opaque);

    /**
     * 回显函数。
     * 
     * @return -1 终止，1 继续。
    */
    int (*dump_cb)(good_allocator_t *alloc, void *opaque);

    /**
    * 环境指针。
    */
    void *opaque;
    
}good_map_t;

/**
 * MAP的索引。
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
 * @return > 0 is data1 > data2，0 is data1 == data2，< 0 is data1 < data2。
 * 
 * @see memcmp()
*/
int good_map_compare(const void *data1, const void *data2, size_t size,void *opaque);

/**
 * 销毁。
 * 
 * @see good_tree_free()
 * @see memset()
*/
void good_map_destroy(good_map_t* map);

/**
 * 初始化。
 * 
 * @return 0 成功，!0 失败。
 * 
 * @see good_tree_alloc2()
 * @see good_map_hash()
 * @see good_map_compare()
*/
int good_map_init(good_map_t* map,size_t size);

/**
 * 查找或创建。
 * 
 * @param ksize Key size。
 * @param vsize Value size。 0 仅查找，>0 不存在则创建。
 * 
 * @return !NULL(0) 成功(复制的指针，不需要主动释放)，NULL(0) 不存在或创建失败。
 * 
 * @see good_tree_alloc3()
 * @see good_tree_insert2()
 * @see good_tree_child()
 * @see good_tree_sibling()
 * @see good_tree_alloc2()
 * @see memcpy()
 * @see good_allocator_atfree()
 * @see good_allocator_refer()
 * 
*/
good_allocator_t* good_map_find(good_map_t* map,const void* key,size_t ksize,size_t vsize);

/**
 * 擦除。
 * 
 * @see good_map_find()
*/
void good_map_erase(good_map_t* map,const void* key,size_t ksize);

/**
 * 扫描节点。
 * 
 * 深度优先遍历节点。
 * 
 * @see good_tree_scan()
*/
void good_map_scan(const good_map_t *map);

#endif //GOOD_UTIL_MAP_H