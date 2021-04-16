/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "buffer.h"

good_buffer_t *good_buffer_alloc(good_allocator_t *alloc)
{
    good_buffer_t *buf = NULL;

    buf = good_heap_alloc(sizeof(good_buffer_t));
    if (!buf)
        return NULL;

    if (alloc)
    {
        assert(alloc->numbers > 0 && alloc->pptrs[0] != NULL && alloc->sizes[0] > 0);

        /*
        * 绑定内存块。
        */
        buf->alloc = alloc;

        buf->data = buf->alloc->pptrs[0];
        buf->size = buf->alloc->sizes[0];

        buf->rsize = buf->wsize = 0;
    }
    else
    {
        /*
         * 允许空的。
        */
        buf->alloc = buf->data = NULL;
        buf->size = buf->rsize = buf->wsize = 0;
    }

    return buf;
}

good_buffer_t *good_buffer_alloc2(size_t size)
{
    good_buffer_t *buf = NULL;
    good_allocator_t *alloc = NULL;

    assert(size > 0);

    alloc = good_allocator_alloc2(size);
    if (!alloc)
        return NULL;

    buf = good_buffer_alloc(alloc);
    if (!buf)
        goto final_error;

    return buf;

final_error:

    good_allocator_unref(&alloc);

    return NULL;
}

void good_buffer_freep(good_buffer_t **dst)
{
    good_buffer_t *buf_p = NULL;

    if (!dst || !*dst)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    buf_p = *dst;

    good_allocator_unref(&buf_p->alloc);

    good_heap_freep((void **)dst);
}

good_buffer_t *good_buffer_copy(good_buffer_t *src)
{
    good_buffer_t *buf = NULL;

    assert(src);
    assert(src->data && src->size > 0);

    /*
     * 如果不支持引用，则执行克隆。
    */
    if (!src->alloc)
        return buf = good_buffer_clone(src);

    buf = good_buffer_alloc(good_allocator_refer(src->alloc));
    if (!buf)
        return NULL;

    buf->rsize = src->rsize;
    buf->wsize = src->wsize;

    return buf;
}

good_buffer_t *good_buffer_clone(good_buffer_t *src)
{
    good_buffer_t *buf = NULL;

    assert(src);
    assert(src->data && src->size > 0);

    buf = good_buffer_alloc2(src->size);
    if (!buf)
        return NULL;

    buf->size = src->size;
    buf->rsize = src->rsize;
    buf->wsize = src->wsize;
    memcpy(buf->data, src->data, src->size);

    return buf;
}

int good_buffer_privatize(good_buffer_t *dst)
{
    good_allocator_t *new_p = NULL;

    assert(dst);

    if (dst->alloc)
    {
        new_p = good_allocator_privatize(&dst->alloc);
        if (!new_p)
            GOOD_ERRNO_AND_RETURN1(ENOMEM, -1);

        /*
         * 旧的指针换成新的指针。
        */
        dst->alloc = new_p;

        dst->data = new_p->pptrs[0];
        dst->size = new_p->sizes[0];
    }

    return 0;
}

int good_buffer_resize(good_buffer_t *buf, size_t size)
{
    good_allocator_t *alloc_new = NULL;

    assert(buf != NULL && size > 0);

    if (buf->size == size)
        return 0;

    alloc_new = good_allocator_alloc2(size);
    if (!alloc_new)
        return -1;

    /*
     * 复制数据。
    */
    memcpy(alloc_new->pptrs[0], buf->data, buf->size);

    /*
     * 解除旧的内存块。
     */
    good_allocator_unref(&buf->alloc);

    /*
     * 绑定新的内存块。
    */
    buf->alloc = alloc_new;

    buf->data = alloc_new->pptrs[0];
    buf->size = alloc_new->sizes[0];

    if (buf->wsize > size)
        buf->wsize = size;
    if (buf->rsize > size)
        buf->rsize = size;

    return 0;
}

ssize_t good_buffer_write(good_buffer_t *buf, const void *data, size_t size)
{
    ssize_t wsize2 = 0;

    if (good_buffer_privatize(buf) != 0)
        GOOD_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL && data != NULL && size > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = GOOD_MIN(buf->size - buf->wsize, size);
    memcpy(GOOD_PTR2PTR(void, buf->data, buf->wsize), data, wsize2);
    buf->wsize += wsize2;

    return wsize2;
}

ssize_t good_buffer_read(good_buffer_t *buf, void *data, size_t size)
{
    ssize_t rsize2 = 0;

    assert(buf != NULL && data != NULL && size > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->rsize >= buf->wsize)
        GOOD_ERRNO_AND_RETURN1(ESPIPE, 0);

    rsize2 = GOOD_MIN(buf->wsize - buf->rsize, size);
    memcpy(data, GOOD_PTR2PTR(void, buf->data, buf->rsize), rsize2);
    buf->rsize += rsize2;

    return rsize2;
}

void good_buffer_drain(good_buffer_t *buf)
{
    assert(good_buffer_privatize(buf) == 0);

    assert(buf != NULL);
    assert(buf->data != NULL && buf->size > 0);

    assert(buf->rsize <= buf->wsize);

    if (buf->rsize > 0)
    {
        buf->wsize -= buf->rsize;
        memmove(buf->data, GOOD_PTR2PTR(void, buf->data, buf->rsize), buf->wsize);
        buf->rsize = 0;
    }
}

ssize_t good_buffer_fill(good_buffer_t *buf, uint8_t stuffing)
{
    ssize_t wsize2 = 0;

    if (good_buffer_privatize(buf) != 0)
        GOOD_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = buf->size - buf->wsize;
    memset(GOOD_PTR2PTR(void, buf->data, buf->wsize), stuffing, wsize2);
    buf->wsize += wsize2;

    return wsize2;
}

ssize_t good_buffer_vprintf(good_buffer_t *buf, const char *fmt, va_list args)
{
    ssize_t wsize2 = 0;

    if (good_buffer_privatize(buf) != 0)
        GOOD_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL && fmt != NULL && args != NULL);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = vsnprintf(GOOD_PTR2PTR(char, buf->data, buf->wsize),
                       buf->size - buf->wsize, fmt, args);
    if (wsize2 > 0)
        buf->wsize += wsize2;

    return wsize2;
}

ssize_t good_buffer_printf(good_buffer_t *buf, const char *fmt, ...)
{
    ssize_t wsize2 = 0;

    assert(buf != NULL && fmt != NULL);
    assert(buf->data != NULL && buf->size > 0);

    va_list args;
    va_start(args, fmt);

    wsize2 = good_buffer_vprintf(buf, fmt, args);

    va_end(args);

    return wsize2;
}

ssize_t good_buffer_import(good_buffer_t *buf, int fd)
{
    struct stat attr = {0};

    assert(buf != NULL && fd >= 0);

    if (fstat(fd, &attr) == -1)
        GOOD_ERRNO_AND_RETURN1(EBADF, -1);

    return good_buffer_import_atmost(buf, fd, attr.st_size);
}

ssize_t good_buffer_import_atmost(good_buffer_t *buf, int fd, size_t howmuch)
{
    ssize_t wsize2 = 0;
    ssize_t wsize3 = 0;

    if (good_buffer_privatize(buf) != 0)
        GOOD_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL && fd >= 0 && howmuch > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = GOOD_MIN(buf->size - buf->wsize, howmuch);
    wsize3 = good_read(fd, GOOD_PTR2PTR(void, buf->data, buf->wsize), wsize2);
    if (wsize3 > 0)
        buf->wsize += wsize3;

    return wsize3;
}

ssize_t good_buffer_export(good_buffer_t *buf, int fd)
{
    return good_buffer_export_atmost(buf, fd, INT16_MAX);
}

ssize_t good_buffer_export_atmost(good_buffer_t *buf, int fd, size_t howmuch)
{
    ssize_t rsize2 = 0;
    ssize_t rsize3 = 0;

    assert(buf != NULL && fd >= 0 && howmuch > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->rsize >= buf->wsize)
        GOOD_ERRNO_AND_RETURN1(ESPIPE, 0);

    rsize2 = GOOD_MIN(buf->wsize - buf->rsize, howmuch);
    rsize3 = good_write(fd, GOOD_PTR2PTR(void, buf->data, buf->rsize), rsize2);
    if (rsize3 > 0)
        buf->rsize += rsize3;

    return rsize3;
}