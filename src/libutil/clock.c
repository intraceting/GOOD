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

int _good_clock_create(void *opaque)
{
    return pthread_key_create((pthread_key_t *)opaque, _good_clock_destroy);
}

good_clock_t *_good_clock_init(uint64_t set)
{
    static atomic_int init = 0;
    static pthread_key_t key = -1;

    int chk = 0;
    good_clock_t *ctx = NULL;

    chk = good_once(&init, _good_clock_create, &key);
    if (chk < 0)
        return NULL;

    ctx = pthread_getspecific(key);
    if (!ctx)
    {
        ctx = good_heap_alloc(sizeof(good_clock_t));
        if (!ctx)
            return NULL;

        chk = pthread_setspecific(key, ctx);
        if (chk != 0)
            good_heap_freep((void **)&ctx);
    }

    if (ctx && chk == 0)
        ctx->point = ctx->start = set;

    return ctx;
}

void good_clock_reset()
{
    uint64_t current = good_time_clock2kind_with(CLOCK_MONOTONIC,6);
    good_clock_t* ctx = _good_clock_init(current);

    assert(ctx);
    
    ctx->start = ctx->point = current;
}

uint64_t good_clock_dot(uint64_t *step)
{
    uint64_t current = good_time_clock2kind_with(CLOCK_MONOTONIC,6);
    good_clock_t* ctx = _good_clock_init(current);

    assert(ctx);

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