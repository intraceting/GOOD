/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "allocator.h"

/**
 * 内存块信息结构。
 * 
 * 包括头部和数据块，数据块紧挨着头部，两部分之间没有填充物。
 * 
 * 申请内存块时，头部和数据块一次性申请创建。
 * 释放内存块时，直接通过头部首地址一次性释放。
*/
typedef struct _good_allocator
{
    /**
     * 魔法数
    */
    uint32_t magic;

    /**
     * 'GOOD' ASCII
    */
#define GOOD_ALLOC_MAGIC 0x474F4F44

    /**
    * 引用计数
    */
    int refcount;

    /**
    * 内存块大小，但不包括内存块头。
    */
    size_t size;

    /**
    * 构造回调。
    */
    void (*construct_cb)(void *m, size_t size, void *opaque);

    /**
    * 析构回调。
    */
    void (*destroy_cb)(void *m, size_t size, void *opaque);

    /**
    * 环境指针。
    */
    void *opaque;


} good_allocator_t;

/*外部指针转内部指针*/
#define GOOD_ALLOC_PTR_OUT2IN(PTR)    \
            GOOD_PTR2PTR(good_allocator_t, (PTR), -sizeof(good_allocator_t))

/*内部指针转外部指针*/
#define GOOD_ALLOC_PTR_IN2OUT(PTR)    \
            GOOD_PTR2PTR(void, (PTR), sizeof(good_allocator_t))


size_t good_size(void *mem)
{
    good_allocator_t *in_p = NULL;

    assert(mem);

    in_p = GOOD_ALLOC_PTR_OUT2IN(mem);

    assert(in_p->magic == GOOD_ALLOC_MAGIC);

    return in_p->size;
}

void *good_alloc(size_t size,
                 void (*construct_cb)(void *m, size_t size, void *opaque),
                 void (*destroy_cb)(void *m, size_t size, void *opaque),
                 void *opaque)
{
    good_allocator_t* m = NULL;
    void* out_p = NULL;

    assert(size > 0);

    m = (good_allocator_t *)calloc(1, sizeof(good_allocator_t) + size);
    if(!m)
        return NULL;

    m->magic = GOOD_ALLOC_MAGIC;
    atomic_init(&m->refcount, 1);
    m->size = size;
    out_p = GOOD_ALLOC_PTR_IN2OUT(m);

    m->construct_cb = construct_cb;
    m->destroy_cb = destroy_cb;
    m->opaque = opaque;

    if(m->construct_cb)
        m->construct_cb(out_p,size,m->opaque);

    return out_p;
}

void *good_refer(void *mem)
{
    good_allocator_t *in_p = NULL;

    assert(mem);

    in_p = GOOD_ALLOC_PTR_OUT2IN(mem);

    assert(in_p->magic == GOOD_ALLOC_MAGIC);

    assert(atomic_fetch_add_explicit(&in_p->refcount, 1, memory_order_acq_rel) > 0);

    return mem;
}

void good_unref(void **mem)
{
    good_allocator_t *in_p = NULL;

    if (!mem || !*mem)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    in_p = GOOD_ALLOC_PTR_OUT2IN(mem);

    assert(in_p->magic == GOOD_ALLOC_MAGIC);

    /*
    * 为1时释放，因为申请时初始化为1。
    */
    if (atomic_fetch_add_explicit(&in_p->refcount, -1, memory_order_acq_rel) == 1)
    {
        if (in_p->destroy_cb)
            in_p->destroy_cb(*mem,in_p->size,in_p->opaque);

        in_p->magic = ~(GOOD_ALLOC_MAGIC);
        in_p->size = 0;
        in_p->construct_cb = NULL;
        in_p->destroy_cb = NULL;
        in_p->opaque = NULL;

        /*
         * 只要释放一次即可全部释放，因为内存是一次申请的。
        */
        free(in_p);
    }

    /*Set to NULL(0)*/
    *mem = NULL;
}