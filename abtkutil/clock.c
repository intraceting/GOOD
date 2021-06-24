/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "clock.h"

/**
 * 计时器
*/
typedef struct _abtk_clock
{
    /**
     * 起始
    */
    uint64_t start;

    /**
     * 打点
    */
    uint64_t point;

} abtk_clock_t;

void _abtk_clock_destroy(void* buf)
{
    if(buf)
        abtk_heap_free(buf);
}

int _abtk_clock_create(void *opaque)
{
    return pthread_key_create((pthread_key_t *)opaque, _abtk_clock_destroy);
}

abtk_clock_t *_abtk_clock_init(uint64_t set)
{
    static int init = 0;
    static pthread_key_t key = -1;

    int chk = 0;
    abtk_clock_t *ctx = NULL;

    chk = abtk_once(&init, _abtk_clock_create, &key);
    if (chk < 0)
        return NULL;

    ctx = pthread_getspecific(key);
    if (!ctx)
    {
        ctx = abtk_heap_alloc(sizeof(abtk_clock_t));
        if (!ctx)
            return NULL;

        chk = pthread_setspecific(key, ctx);
        if (chk != 0)
            abtk_heap_freep((void **)&ctx);
    }

    if (ctx && chk == 0)
        ctx->point = ctx->start = set;

    return ctx;
}

void abtk_clock_reset()
{
    uint64_t current = abtk_time_clock2kind_with(CLOCK_MONOTONIC,6);
    abtk_clock_t* ctx = _abtk_clock_init(current);

    assert(ctx);
    
    ctx->start = ctx->point = current;
}

uint64_t abtk_clock_dot(uint64_t *step)
{
    uint64_t current = abtk_time_clock2kind_with(CLOCK_MONOTONIC,6);
    abtk_clock_t* ctx = _abtk_clock_init(current);

    assert(ctx);

    uint64_t dot = current - ctx->start;

    if (step)
        *step = current - ctx->point;

    ctx->point = current;

    return dot;
}

uint64_t abtk_clock_step(uint64_t *dot)
{
    uint64_t ldot = 0, step = 0;
    
    ldot = abtk_clock_dot(&step);

    if(dot)
        *dot = ldot;
    
    return step;
}