/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_POOL_H
#define ABTKUTIL_POOL_H

#include "general.h"
#include "allocator.h"
#include "thread.h"

/**
 * 一个简单的池子。
*/
typedef struct _abtk_pool
{
    /**
     * 池子。
     * 
     * @note 尽量不要直接修改。
    */
    abtk_allocator_t *table;

    /**
     * 队列长度。
     * 
     * @note 尽量不要直接修改。
    */
    size_t count;

    /**
     * 拉取游标。
     * 
     * @note 尽量不要直接修改。
    */
    size_t pull_pos;

    /**
     * 推送游标。
     * 
     * @note 尽量不要直接修改。
    */
    size_t push_pos;

} abtk_pool_t;

/**
 * 销毁。
 * 
 * @warning 所有内存块必须全部在池内才能被销毁。
*/
void abtk_pool_destroy(abtk_pool_t *pool);

/**
 * 初始化。
 * 
 * @param size 大小。
 * @param number 数量。
 * 
 * @return 0 成功，!0 失败。
*/
int abtk_pool_init(abtk_pool_t *pool, size_t size, size_t number);

/**
 * 拉取数据。
 * 
 * @return >= 0 成功(读取数据长度)，< 0 失败(空了)。
 * 
*/
ssize_t abtk_pool_pull(abtk_pool_t *pool, void *buf, size_t size);

/**
 * 推送数据。
 * 
 * @return >= 0 成功(写入数据长度)，< 失败(满了)。
 * 
*/
ssize_t abtk_pool_push(abtk_pool_t *pool, const void *buf, size_t size);

#endif //ABTKUTIL_POOL_H