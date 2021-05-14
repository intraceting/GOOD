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

    good_heap_freep((void**)&pool->table);
    good_heap_freep((void**)&pool->queue);

    good_mutex_destroy(&pool->mutex);

    memset(pool,0,sizeof(*pool));
}

int good_pool_init(good_pool_t *pool,size_t size)
{
    assert(pool != NULL && size > 0);

    pool->table = (uint8_t*)good_heap_alloc(good_align(size / 8, 2)); //余数要补上1。
    pool->queue = (ssize_t*)good_heap_alloc(size*sizeof(ssize_t));

    if (!pool->table || !pool->queue)
    {
        good_heap_freep((void**)&pool->table);
        good_heap_freep((void**)&pool->queue);

        return -1;
    }

    good_mutex_init2(&pool->mutex,0);

    pool->size = size;
    pool->count = 0;
    pool->pull_pos = 0;
    pool->push_pos = 0;
    pool->enable = 1;

    return 0;
}

ssize_t good_pool_pull(good_pool_t* pool, time_t timeout)
{
    ssize_t id = -1;
    int chk;

    assert(pool != NULL);

    assert(good_mutex_lock(&pool->mutex,0)==0);

    while (pool->enable)
    {
        if (pool->count <= 0)
        {
            chk = good_mutex_wait(&pool->mutex, timeout);
            if (chk != 0)
                break;
        }

        if (pool->count <= 0)
            continue;

        /*从队列取出一个ID。*/
        id = pool->queue[pool->pull_pos];
        pool->queue[pool->pull_pos] = -1;//用-1覆盖。

        /*队列长度减去1。*/
        pool->count -= 1;

        /*索引表清除ID，防止重复引用或错误的引用。*/
        assert(good_bloom_unset(pool->table, good_align(pool->size / 8, 2), id - 1) == 0);

        /*滚动游标。*/
        pool->pull_pos = (pool->pull_pos + 1) % pool->size;

        /*退出循环。*/
        break;
    }

    assert(good_mutex_unlock(&pool->mutex)==0);

    return id;
}

int good_pool_push(good_pool_t *pool, ssize_t id)
{
    int chk = -1;

    assert(pool != NULL);

    assert(good_mutex_lock(&pool->mutex, 0) == 0);

    /*ID不能超过池的范围。*/
    if (id <= 0 || id > pool->size)
        goto final;

    /*不能超过池大小。*/
    if (pool->count >= pool->size)
        goto final;

    /*索引表格标记ID，防止重复反引用或错误的反引用。*/
    assert(good_bloom_mark(pool->table, good_align(pool->size / 8, 2), id - 1) == 0);

    /*向队列添加一个ID。*/
    pool->queue[pool->push_pos] = id;

    /*队列长度加1。*/
    pool->count += 1;

    /*滚动游标。*/
    pool->push_pos = (pool->push_pos + 1) % pool->size;

    /*可能有等待的，通知一下。*/
    good_mutex_signal(&pool->mutex,1);

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
    good_mutex_signal(&pool->mutex,1);

    assert(good_mutex_unlock(&pool->mutex) == 0);
}