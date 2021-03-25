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

good_buffer_t *good_buffer_alloc(size_t size[], size_t number,
                                 void (*free_cb)(size_t number, uint8_t **data, size_t *size, void *opaque), 
                                 void *opaque)
{
    good_buffer_hdr *buf_p = NULL;
    uint8_t* data_p = NULL;
    size_t need_size = sizeof(good_buffer_hdr);
    
    need_size +=  number * sizeof(uint8_t *);//data[n]
    need_size +=  number * sizeof(size_t);//size[n]
    need_size +=  number * sizeof(size_t);//size1[n]
    need_size +=  number * sizeof(size_t);//size2[n]

    /*
     * 计算全部的缓存，以减少内存碎片，并且释放时可以简单的直接释放，而且也不会因为外部修改产生内存泄漏。
    */
    if(size != NULL)
    {
        for (size_t i = 0; i < number; i++)
            need_size += size[i];
    }

    buf_p = (good_buffer_hdr *)good_heap_alloc(need_size);

    if (!buf_p)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

    buf_p->magic = GOOD_BUFFER_MAGIC;
    atomic_init(&buf_p->refcount, 1);

    buf_p->out.free_cb = free_cb;
    buf_p->out.opaque = opaque;
    buf_p->out.number = number;

    if (number > 0)
    {
        /*
         * 分配各项地址。
         * data = good_buffer_hdr + 1
         * size = good_buffer_hdr + data[]
         * size1 = good_buffer_hdr + data[] + size[]
         * size2 = good_buffer_hdr + data[] + size[] + size1[]
        */
        buf_p->out.data = GOOD_PTR2PTR(uint8_t*, buf_p, sizeof(good_buffer_hdr));
        buf_p->out.size = GOOD_PTR2PTR(size_t, buf_p->out.data, number * sizeof(uint8_t*));
        buf_p->out.size1 = GOOD_PTR2PTR(size_t, buf_p->out.size, number * sizeof(size_t));
        buf_p->out.size2 = GOOD_PTR2PTR(size_t, buf_p->out.size1, number * sizeof(size_t));

        /*
         * 第一块缓存。
         * data[n] = good_buffer_hdr + data[] + size[] + size1[] + + size2[]
        */
        data_p = GOOD_PTR2PTR(uint8_t, buf_p->out.size2, number * sizeof(size_t));

        if (size != NULL)
        {
            /*
            * 分配缓存地址。
            */
            for (size_t i = 0; i < number; i++)
            {
                /*
                * 跳过不需要分配的。
                */
                if (size[i] <= 0)
                    continue;

                /*
                * 绑定大小和地址。
                */
                buf_p->out.size[i] = size[i];
                buf_p->out.data[i] = data_p;

                /*
                * 下一块。
                */
                data_p = GOOD_PTR2PTR(uint8_t, data_p, size[i]);
            }
        }
    }

    return GOOD_BUFFER_PTR_IN2OUT(buf_p);
}

good_buffer_t *good_buffer_alloc2(size_t size[],size_t number)
{
    return good_buffer_alloc(size, number, NULL, NULL);
}

good_buffer_t *good_buffer_alloc3(size_t size)
{
    return good_buffer_alloc2(&size,1);
}

good_buffer_t *good_buffer_refer(good_buffer_t *buf)
{
    good_buffer_hdr *buf_p = NULL;

    assert(buf);

    buf_p = GOOD_BUFFER_PTR_OUT2IN(buf);

    assert(buf_p->magic == GOOD_BUFFER_MAGIC);

    assert(atomic_fetch_add_explicit(&buf_p->refcount, 1, memory_order_acq_rel) > 0);

    return buf;
}

void good_buffer_unref(good_buffer_t **buf)
{
    good_buffer_hdr *buf_p = NULL;

    if (!buf || !*buf)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    buf_p = GOOD_BUFFER_PTR_OUT2IN(*buf);

    assert(buf_p->magic == GOOD_BUFFER_MAGIC);

    if (atomic_fetch_add_explicit(&buf_p->refcount, -1, memory_order_acq_rel) == 1)
    {
        if (buf_p->out.free_cb)
            buf_p->out.free_cb(buf_p->out.number,buf_p->out.data,buf_p->out.size,buf_p->out.opaque);

        buf_p->magic = ~(GOOD_BUFFER_MAGIC);
        buf_p->out.size1 = NULL;
        buf_p->out.size = NULL;
        buf_p->out.data = NULL;
        buf_p->out.number = 0;
        buf_p->out.free_cb = NULL;
        buf_p->out.opaque = NULL;

        /*
         * 只要释放一次即可全部释放，因为内存是一次申请的。
        */
        good_heap_freep((void**)&buf_p);
    }

    /*Set to NULL(0)*/
    *buf = NULL;
}