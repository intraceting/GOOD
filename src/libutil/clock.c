/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "clock.h"

/**
 * 计时器
*/
typedef struct _good_clock
{
    /**
     * 起始
    */
    uint64_t start;

    /**
     * 打点
    */
    uint64_t point;

} good_clock_t;

void _good_clock_destroy(void* buf)
{
    if(buf)
        good_heap_free(buf);
}

pthread_key_t* _good_clock_init()
{
    static atomic_int init = 0;
    static pthread_key_t key = -1;

    good_clock_t* ctx = NULL;
    atomic_int cmp = 0;
    int chk;

    if(atomic_compare_exchange_strong(&init,&cmp,1))
    {
        chk = pthread_key_create(&key,_good_clock_destroy);
        
        atomic_store(&init,((chk == 0)?2:0));
    }
    else
    {
        while(atomic_load(&init) == 1)
            pthread_yield();
    }

    assert(atomic_load(&init) == 2);

    return &key;
}


void good_clock_reset()
{
    good_clock_t* ctx = _good_clock_init();

    ctx = (good_clock_t *)pthread_getspecific(key);
    if (!ctx)
    {
        ctx = good_heap_alloc(sizeof(good_clock_t));

        chk = pthread_setspecific(key, ctx);

        assert(chk == 0);
    }

    ctx->start = ctx->point = good_time_clock2kind_with(CLOCK_MONOTONIC,6);
}

uint64_t good_clock_dot(uint64_t *step)
{
    good_clock_t* ctx = _good_clock_init();

    uint64_t current = good_time_clock2kind_with(CLOCK_MONOTONIC,6);
    uint64_t dot = current - ctx->start;

    if (step)
        *step = current - ctx->point;

    ctx->point = current;

    return dot;
}

uint64_t good_clock_step(uint64_t *dot)
{
    uint64_t ldot = 0, step = 0;
    
    ldot = good_clock_dot(&step);

    if(dot)
        *dot = ldot;
    
    return step;
}