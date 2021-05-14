/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "pool.h"

void good_pool_destroy(good_pool_t* pool)
{
    assert(pool != NULL);

    assert(pool->count == pool->queue->numbers);

    good_allocator_unref(&pool->table);
    good_allocator_unref(&pool->queue);

    good_mutex_destroy(&pool->mutex);

    memset(pool,0,sizeof(*pool));
}

int good_pool_init(good_pool_t *pool, size_t size, size_t number)
{
    size_t id_size = sizeof(size_t);

    assert(pool != NULL && size > 0 && number > 0);

    pool->table = good_allocator_alloc2(good_align(number / 8, 2)); //余数要补上1。
    pool->queue = good_allocator_alloc(&id_size,number,1);

    if (!pool->table || !pool->queue)
    {
        good_allocator_unref(&pool->table);
        good_allocator_unref(&pool->queue);

        return -1;
    }

    good_mutex_init2(&pool->mutex,0);

    pool->count = 0;
    pool->refer_pos = 0;
    pool->unref_pos = 0;
    pool->enable = 1;

    /*初始化表格和队列。*/
    for (; pool->count < pool->queue->numbers; pool->count++)
    {
        /*填充索引表格。*/
        assert(good_bloom_mark(pool->table->pptrs[0], pool->table->sizes[0], pool->unref_pos)==0);

        /*填充队列ID。*/
        GOOD_PTR2SIZE(pool->queue->pptrs[pool->unref_pos], 0) = pool->count + 1; //begin 1.

        /*滚动游标。*/
        pool->unref_pos = (pool->unref_pos + 1) % pool->queue->numbers;
    }

    return 0;
}

ssize_t good_pool_refer(good_pool_t* pool,int peek)
{
    ssize_t id = -1;

    assert(pool != NULL);

    assert(good_mutex_lock(&pool->mutex,0)==0);

    while (pool->enable)
    {
        if (pool->count <= 0)
        {
            if (peek)
                break;
            else
                good_mutex_wait(&pool->mutex, -1);
        }

        if (pool->count <= 0)
            continue;

        /*从队列取出一个ID。*/
        id = GOOD_PTR2SIZE(pool->queue->pptrs[pool->refer_pos], 0);
        GOOD_PTR2SIZE(pool->queue->pptrs[pool->refer_pos], 0) = -1;//用-1覆盖。

        /*队列长度减去1。*/
        pool->count -= 1;

        /*索引表清除ID，防止重复引用或错误的引用。*/
        assert(good_bloom_unset(pool->table->pptrs[0], pool->table->sizes[0], id - 1) == 0);

        /*滚动游标。*/
        pool->refer_pos = (pool->refer_pos + 1) % pool->queue->numbers;

        /*退出循环。*/
        break;
    }

    assert(good_mutex_unlock(&pool->mutex)==0);

    return id;
}

int good_pool_unref(good_pool_t *pool, ssize_t id)
{
    int chk = -1;

    assert(pool != NULL);

    assert(good_mutex_lock(&pool->mutex, 0) == 0);

    /*ID不能超过池的范围。*/
    if (id <= 0 || id > pool->queue->numbers)
        goto final;

    /*索引表格标记ID，防止重复反引用或错误的反引用。*/
    assert(good_bloom_mark(pool->table->pptrs[0], pool->table->sizes[0], id - 1) == 0);

    /*向队列添加一个ID。*/
    GOOD_PTR2SIZE(pool->queue->pptrs[pool->unref_pos], 0) = id;

    /*队列长度加1。*/
    pool->count += 1;

    /*滚动游标。*/
    pool->unref_pos = (pool->unref_pos + 1) % pool->queue->numbers;

    /*可能有等待的，通知一下。*/
    good_mutex_notice(&pool->mutex,1);

    /*No error.*/
    chk = 0;

final:

    assert(good_mutex_unlock(&pool->mutex) == 0);

    return chk;
}

void good_pool_enable(good_pool_t *pool,int enable)
{
    assert(pool != NULL);

    assert(good_mutex_lock(&pool->mutex, 0) == 0);

    /**/
    pool->enable = (enable ? 1 : 0);

    /*可能有等待的，通知一下。*/
    good_mutex_notice(&pool->mutex,1);

    assert(good_mutex_unlock(&pool->mutex) == 0);
}