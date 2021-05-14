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
 * 一个简单的池子。
*/
typedef struct _good_pool
{
    /**
     * 互斥量和事件。
    */
    good_mutex_t mutex;

    /**
     * 大小。
     * 
     * @note 尽量不要直接修改。
    */
    size_t size;

    /**
     * 索引表。
     * 
     * @note 尽量不要直接修改。
    */
    uint8_t *table;

    /**
     * 索引队列。
     * 
     * @note 尽量不要直接修改。
    */
    ssize_t *queue;

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
 * 初始化一个空的池子。
 * 
 * @param size 大小。
 * 
 * @return 0 成功，!0 失败。
*/
int good_pool_init(good_pool_t* pool,size_t size);

/**
 * 拉取一个ID。
 * 
 * @param timeout 超时(毫秒)。>= 0 成功或时间过期后返回，< 0 成功或出错后返回。
 * 
 * @return > 0 成功(索引ID)，<= 0 失败。
*/
ssize_t good_pool_pull(good_pool_t *pool, time_t timeout);

/**
 * 推送一个ID。
 * 
 * @param id 索引ID。
 * 
 * @return  0 成功，-1 失败。
 * 
 * @warning 不受使能开关影响。
*/
int good_pool_push(good_pool_t *pool, ssize_t id);

/** 
 * 设置使能开关状态。
 * 
 * @param enable !0 启用，0 禁用。
*/
void good_pool_enable(good_pool_t *pool,int enable);

#endif //GOOD_UTIL_POOL_H