/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_POOL_H
#define GOOD_UTIL_POOL_H

#include "general.h"
#include "allocator.h"
#include "thread.h"

/**
 * 
*/
typedef struct _good_pool
{
    /**
     * 互斥量和事件。
    */
    good_mutex_t mutex;

    /**
     * 内存块。
     * 
     * @note 尽量不要直接修改。 
    */
    good_allocator_t *alloc;

    /**
     * 忙碌的内存块索引。
     * 
     * @note 尽量不要直接修改。
    */
    good_allocator_t *busy_index;

    /**
     * 忙碌的内存块数量。
    */
    size_t busy_count;

    /**
     * 忙碌的内存块游标。
    */
    size_t busy_cursor;

    /**
     * 空闲的内存块索引。
     * 
     * @note 尽量不要直接修改。
    */
    good_allocator_t *idle_index;

    /**
     * 空闲的内存块数量。
    */
    size_t idle_count;

    /**
     * 空闲的内存块游标。
    */
    size_t idle_cursor;


} good_pool_t;

/**
 * 销毁。
*/
void good_pool_destroy(good_pool_t* pool);

/**
 * 初始化。
 * 
 * @return 0 成功，!0 失败。
*/
int good_pool_init(good_pool_t* pool,size_t size,size_t number);

#endif //GOOD_UTIL_POOL_H