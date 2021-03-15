/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "buffer.h"

/**
 * 缓存头部
*/
typedef struct _good_buffer_hdr
{
    /**
     * 魔法数
    */
    uint32_t magic;

    /**
     * 'GOOD' ASCII
    */
#define GOOD_BUFFER_MAGIC 0x474F4F44

    /**
    * 引用计数
    */
    int refcount;

    /**
     * 缓存
     * 
     * @note 必须是最后一个。
    */
    good_buffer_t out;

} good_buffer_hdr;


#define GOOD_BUFFER_PTR_OUT2IN(PTR)    \
            GOOD_PTR2PTR(good_buffer_hdr, (PTR), -(sizeof(good_buffer_hdr)-sizeof(good_buffer_t)))

#define GOOD_BUFFER_PTR_IN2OUT(PTR)    \
            GOOD_PTR2PTR(good_buffer_t, (PTR), (sizeof(good_buffer_hdr)-sizeof(good_buffer_t)))


good_buffer_t *good_buffer_alloc(size_t size, void (*free_cb)(void *data, void *opaque), void *opaque)
{
    good_buffer_hdr *buf_p = NULL;

    buf_p = (good_buffer_hdr *)good_heap_alloc(sizeof(good_buffer_hdr) + size);

    if (!buf_p)
        return NULL;

    buf_p->magic = GOOD_BUFFER_MAGIC;
    atomic_init(&buf_p->refcount, 1);

    buf_p->out.size = size;
    if (size > 0)
        buf_p->out.data = GOOD_PTR2PTR(void, buf_p, sizeof(good_buffer_hdr));// good_buffer_hdr + 1

    buf_p->out.free_cb = free_cb;
    buf_p->out.opaque = opaque;

    return GOOD_BUFFER_PTR_IN2OUT(buf_p);
}

good_buffer_t *good_buffer_alloc2(size_t size)
{
    return good_buffer_alloc(size, NULL, NULL);
}

good_buffer_t *good_buffer_refer(good_buffer_t *buf)
{
    good_buffer_hdr *buf_p = NULL;

    if (!buf)
        return NULL;

    buf_p = GOOD_BUFFER_PTR_OUT2IN(buf);

    assert(buf_p->magic == GOOD_BUFFER_MAGIC);

    assert(atomic_fetch_add_explicit(&buf_p->refcount, 1, memory_order_acq_rel) > 0);

    return buf;
}

void good_buffer_unref(good_buffer_t **buf)
{
    good_buffer_hdr *buf_p = NULL;

    if (!buf || !*buf)
        return;

    buf_p = GOOD_BUFFER_PTR_OUT2IN(*buf);

    assert(buf_p->magic == GOOD_BUFFER_MAGIC);

    if (atomic_fetch_add_explicit(&buf_p->refcount, -1, memory_order_acq_rel) == 1)
    {
        if (buf_p->out.free_cb)
            buf_p->out.free_cb(buf_p->out.data, buf_p->out.opaque);

        buf_p->magic = ~(GOOD_BUFFER_MAGIC);
        buf_p->out.size = 0;
        buf_p->out.data = NULL;
        buf_p->out.free_cb = NULL;
        buf_p->out.opaque = NULL;

        good_heap_freep((void**)&buf_p);
    }

    /*Set to NULL(0)*/
    *buf = NULL;
}