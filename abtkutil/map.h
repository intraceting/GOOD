/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_MAP_H
#define ABTKUTIL_MAP_H

#include "general.h"
#include "tree.h"

/**
 * MAP。
 * 
 * HASH和DEQUE存储结构。
*/
typedef struct _abtk_map
{
    /**
     * 表格。
     * 
     * @note 尽量不要直接修改。
    */
    abtk_tree_t *table;

    /**
     * KEY哈希函数。
    */
    uint64_t (*hash_cb)(const void* key,size_t size,void *opaque);

    /**
     * KEY比较函数。
    */
    int (*compare_cb)(const void *key1, const void *key2, size_t size,void *opaque);

    /** 
     * 构造函数。
    */
    void (*construct_cb)(abtk_allocator_t *alloc, void *opaque);

    /**
     * 析构函数。
    */
    void (*destructor_cb)(abtk_allocator_t *alloc, void *opaque);

    /**
     * 回显函数。
     * 
     * @return -1 终止，1 继续。
    */
    int (*dump_cb)(abtk_allocator_t *alloc, void *opaque);

    /**
    * 环境指针。
    */
    void *opaque;
    
}abtk_map_t;

/**
 * MAP的字段。
*/
enum _abtk_map_field
{
    /**
     * Bucket 字段索引。
    */
   ABTK_MAP_BUCKET = 0,
#define ABTK_MAP_BUCKET     ABTK_MAP_BUCKET

    /**
     * Key 字段索引。
    */
   ABTK_MAP_KEY = 0,
#define ABTK_MAP_KEY        ABTK_MAP_KEY

    /**
     * Value 字段索引。
    */
   ABTK_MAP_VALUE = 1
#define ABTK_MAP_VALUE      ABTK_MAP_VALUE
};

/**
 * HASH函数。
*/
uint64_t abtk_map_hash(const void* data,size_t size,void *opaque);

/**
 * 比较函数。
 * 
 * @return > 0 is data1 > data2，0 is data1 == data2，< 0 is data1 < data2。
*/
int abtk_map_compare(const void *data1, const void *data2, size_t size,void *opaque);

/**
 * 销毁。
*/
void abtk_map_destroy(abtk_map_t* map);

/**
 * 初始化。
 * 
 * @return 0 成功，!0 失败。
*/
int abtk_map_init(abtk_map_t* map,size_t size);

/**
 * 查找或创建。
 * 
 * @param ksize Key size。
 * @param vsize Value size。 0 仅查找，>0 不存在则创建。
 * 
 * @return !NULL(0) 成功(复制的指针，不需要主动释放)，NULL(0) 不存在或创建失败。
 * 
*/
abtk_allocator_t* abtk_map_find(abtk_map_t* map,const void* key,size_t ksize,size_t vsize);

/**
 * 删除。
*/
void abtk_map_remove(abtk_map_t* map,const void* key,size_t ksize);

/**
 * 扫描节点。
 * 
 * 深度优先遍历节点。
*/
void abtk_map_scan(abtk_map_t *map);

#endif //ABTKUTIL_MAP_H