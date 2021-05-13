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

    assert(pool->idle_count + pool->busy_count == pool->alloc->numbers);

    good_allocator_unref(&pool->alloc);
    good_allocator_unref(&pool->busy_index);
    good_allocator_unref(&pool->idle_index);

    good_mutex_destroy(&pool->mutex);

    memset(pool,0,sizeof(*pool));
}

int good_pool_init(good_pool_t *pool, size_t size, size_t number)
{
    size_t idx_size = sizeof(size_t);

    assert(pool != NULL && size > 0 && number > 0);

    pool->alloc = good_allocator_alloc(&size,number,1);
    pool->busy_index = good_allocator_alloc(&idx_size,number,1);
    pool->idle_index = good_allocator_alloc(&idx_size,number,1);

    if (!pool->alloc || !pool->busy_index || !pool->idle_index)
    {
        good_allocator_unref(&pool->alloc);
        good_allocator_unref(&pool->busy_index);
        good_allocator_unref(&pool->idle_index);

        return -1;
    }

    pool->busy_count = pool->busy_cursor = 0;
    pool->idle_count = pool->idle_cursor = 0;
    good_mutex_init2(&pool->mutex,0);

    return 0;
}
