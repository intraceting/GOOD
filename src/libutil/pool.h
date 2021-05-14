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
    */
    good_allocator_t *alloc;

    /**
     * 索引表。
     * 
     * @note 尽量不要直接修改。
    */
    good_allocator_t *table;

    /**
     * 索引队列。
     * 
     * @note 尽量不要直接修改。
    */
    good_allocator_t *queue;

    /**
     * 队列长度。
     * 
     * @note 尽量不要直接修改。
    */
    size_t count;

    /**
     * 引用游标。
     * 
     * @note 尽量不要直接修改。
    */
    size_t refer_pos;

    /**
     * 反引用游标。
     * 
     * @note 尽量不要直接修改。
    */
    size_t unref_pos;

    /**
     * 使能开关。
     * 
     * !0 启用，0 禁用。
     * 
     * @note 尽量不要直接修改。
    */
    int enable;

} good_pool_t;

/**
 * 销毁。
 * 
 * @warning 所有内存块必须全部在池内才能被销毁。
*/
void good_pool_destroy(good_pool_t* pool);

/**
 * 初始化。
 * 
 * @return 0 成功，!0 失败。
*/
int good_pool_init(good_pool_t* pool,size_t size,size_t number);

/**
 * 引用。
 * 
 * @param peek 0 直到引用成功或出错返回，!0 无论引用成功与否都返回。
 * 
 * @return > 0 成功(内存块ID)，<= 0 失败。
*/
ssize_t good_pool_refer(good_pool_t *pool, int peek);

/**
 * 反引用。
 * 
 * @return  0 成功，-1 失败。
 * 
 * @warning 不受使能开关影响。
*/
int good_pool_unref(good_pool_t *pool, ssize_t id);

/** 
 * 设置使能开关状态。
 * 
 * @param enable !0 启用，0 禁用。
*/
void good_pool_enable(good_pool_t *pool,int enable);

#endif //GOOD_UTIL_POOL_H