/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "buffer.h"

abtk_buffer_t *abtk_buffer_alloc(abtk_allocator_t *alloc)
{
    abtk_buffer_t *buf = NULL;

    buf = abtk_heap_alloc(sizeof(abtk_buffer_t));
    if (!buf)
        return NULL;

    if (alloc)
    {
        assert(alloc->numbers > 0 && alloc->pptrs[0] != NULL && alloc->sizes[0] > 0);

        /*绑定内存块。*/
        buf->alloc = alloc;

        buf->data = buf->alloc->pptrs[0];
        buf->size = buf->alloc->sizes[0];

        buf->rsize = buf->wsize = 0;
    }
    else
    {
        /*允许空的。*/
        buf->alloc = buf->data = NULL;
        buf->size = buf->rsize = buf->wsize = 0;
    }

    return buf;
}

abtk_buffer_t *abtk_buffer_alloc2(size_t size)
{
    abtk_buffer_t *buf = NULL;
    abtk_allocator_t *alloc = NULL;

    if(size > 0)
    {
        alloc = abtk_allocator_alloc2(size);
        if (!alloc)
            goto final_error;
    }

    buf = abtk_buffer_alloc(alloc);
    if (!buf)
        goto final_error;

    return buf;

final_error:

    abtk_allocator_unref(&alloc);

    return NULL;
}

void abtk_buffer_freep(abtk_buffer_t **dst)
{
    abtk_buffer_t *buf_p = NULL;

    if (!dst || !*dst)
        ABTK_ERRNO_AND_RETURN0(EINVAL);

    buf_p = *dst;

    abtk_allocator_unref(&buf_p->alloc);

    abtk_heap_freep((void **)dst);
}

abtk_buffer_t *abtk_buffer_copy(abtk_buffer_t *src)
{
    abtk_buffer_t *buf = NULL;

    assert(src);
    
    if(src->data != NULL && src->size > 0)
    {
        /*如果不支持引用，则执行克隆。*/
        if (!src->alloc)
            return buf = abtk_buffer_clone(src);

        buf = abtk_buffer_alloc(abtk_allocator_refer(src->alloc));
        if (!buf)
            return NULL;

        buf->rsize = src->rsize;
        buf->wsize = src->wsize;
    }
    else
    {
        buf = abtk_buffer_alloc(NULL);
    }

    return buf;
}

abtk_buffer_t *abtk_buffer_clone(abtk_buffer_t *src)
{
    abtk_buffer_t *buf = NULL;

    assert(src);

    if(src->data != NULL && src->size > 0)
    {
        buf = abtk_buffer_alloc2(src->size);
        if (!buf)
            return NULL;

        buf->size = src->size;
        buf->rsize = src->rsize;
        buf->wsize = src->wsize;
        memcpy(buf->data, src->data, src->size);
    }
    else
    {
        buf = abtk_buffer_alloc(NULL);
    }

    return buf;
}

int abtk_buffer_privatize(abtk_buffer_t *dst)
{
    abtk_allocator_t *new_p = NULL;

    assert(dst);

    if (dst->alloc)
    {
        new_p = abtk_allocator_privatize(&dst->alloc);
        if (!new_p)
            ABTK_ERRNO_AND_RETURN1(ENOMEM, -1);

        /*旧的指针换成新的指针。*/
        dst->alloc = new_p;

        dst->data = new_p->pptrs[0];
        dst->size = new_p->sizes[0];
    }

    return 0;
}

int abtk_buffer_resize(abtk_buffer_t *buf, size_t size)
{
    abtk_allocator_t *alloc_new = NULL;

    assert(buf != NULL && size > 0);

    if (buf->size == size)
        return 0;

    alloc_new = abtk_allocator_alloc2(size);
    if (!alloc_new)
        return -1;

    /*复制数据。*/
    memcpy(alloc_new->pptrs[0], buf->data, buf->size);

    /*解除旧的内存块*/
    abtk_allocator_unref(&buf->alloc);

    /*绑定新的内存块。*/
    buf->alloc = alloc_new;

    buf->data = alloc_new->pptrs[0];
    buf->size = alloc_new->sizes[0];

    if (buf->wsize > size)
        buf->wsize = size;
    if (buf->rsize > size)
        buf->rsize = size;

    return 0;
}

ssize_t abtk_buffer_write(abtk_buffer_t *buf, const void *data, size_t size)
{
    ssize_t wsize2 = 0;

    if (abtk_buffer_privatize(buf) != 0)
        ABTK_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL && data != NULL && size > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        ABTK_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = ABTK_MIN(buf->size - buf->wsize, size);
    memcpy(ABTK_PTR2PTR(void, buf->data, buf->wsize), data, wsize2);
    buf->wsize += wsize2;

    return wsize2;
}

ssize_t abtk_buffer_read(abtk_buffer_t *buf, void *data, size_t size)
{
    ssize_t rsize2 = 0;

    assert(buf != NULL && data != NULL && size > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->rsize >= buf->wsize)
        ABTK_ERRNO_AND_RETURN1(ESPIPE, 0);

    rsize2 = ABTK_MIN(buf->wsize - buf->rsize, size);
    memcpy(data, ABTK_PTR2PTR(void, buf->data, buf->rsize), rsize2);
    buf->rsize += rsize2;

    return rsize2;
}

void abtk_buffer_drain(abtk_buffer_t *buf)
{
    assert(abtk_buffer_privatize(buf) == 0);

    assert(buf != NULL);
    assert(buf->data != NULL && buf->size > 0);

    assert(buf->rsize <= buf->wsize);

    if (buf->rsize > 0)
    {
        buf->wsize -= buf->rsize;
        memmove(buf->data, ABTK_PTR2PTR(void, buf->data, buf->rsize), buf->wsize);
        buf->rsize = 0;
    }
}

ssize_t abtk_buffer_fill(abtk_buffer_t *buf, uint8_t stuffing)
{
    ssize_t wsize2 = 0;

    if (abtk_buffer_privatize(buf) != 0)
        ABTK_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        ABTK_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = buf->size - buf->wsize;
    memset(ABTK_PTR2PTR(void, buf->data, buf->wsize), stuffing, wsize2);
    buf->wsize += wsize2;

    return wsize2;
}

ssize_t abtk_buffer_vprintf(abtk_buffer_t *buf, const char *fmt, va_list args)
{
    ssize_t wsize2 = 0;

    if (abtk_buffer_privatize(buf) != 0)
        ABTK_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL && fmt != NULL && args != NULL);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        ABTK_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = vsnprintf(ABTK_PTR2PTR(char, buf->data, buf->wsize),
                       buf->size - buf->wsize, fmt, args);
    if (wsize2 > 0)
        buf->wsize += wsize2;

    return wsize2;
}

ssize_t abtk_buffer_printf(abtk_buffer_t *buf, const char *fmt, ...)
{
    ssize_t wsize2 = 0;

    assert(buf != NULL && fmt != NULL);
    assert(buf->data != NULL && buf->size > 0);

    va_list args;
    va_start(args, fmt);

    wsize2 = abtk_buffer_vprintf(buf, fmt, args);

    va_end(args);

    return wsize2;
}

ssize_t abtk_buffer_import(abtk_buffer_t *buf, int fd)
{
    struct stat attr = {0};

    assert(buf != NULL && fd >= 0);

    if (fstat(fd, &attr) == -1)
        ABTK_ERRNO_AND_RETURN1(EBADF, -1);

    return abtk_buffer_import_atmost(buf, fd, attr.st_size);
}

ssize_t abtk_buffer_import_atmost(abtk_buffer_t *buf, int fd, size_t howmuch)
{
    ssize_t wsize2 = 0;
    ssize_t wsize3 = 0;

    if (abtk_buffer_privatize(buf) != 0)
        ABTK_ERRNO_AND_RETURN1(EMLINK, -1);

    assert(buf != NULL && fd >= 0 && howmuch > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        ABTK_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = ABTK_MIN(buf->size - buf->wsize, howmuch);
    wsize3 = abtk_read(fd, ABTK_PTR2PTR(void, buf->data, buf->wsize), wsize2);
    if (wsize3 > 0)
        buf->wsize += wsize3;

    return wsize3;
}

ssize_t abtk_buffer_export(abtk_buffer_t *buf, int fd)
{
    return abtk_buffer_export_atmost(buf, fd, INT16_MAX);
}

ssize_t abtk_buffer_export_atmost(abtk_buffer_t *buf, int fd, size_t howmuch)
{
    ssize_t rsize2 = 0;
    ssize_t rsize3 = 0;

    assert(buf != NULL && fd >= 0 && howmuch > 0);
    assert(buf->data != NULL && buf->size > 0);

    if (buf->rsize >= buf->wsize)
        ABTK_ERRNO_AND_RETURN1(ESPIPE, 0);

    rsize2 = ABTK_MIN(buf->wsize - buf->rsize, howmuch);
    rsize3 = abtk_write(fd, ABTK_PTR2PTR(void, buf->data, buf->rsize), rsize2);
    if (rsize3 > 0)
        buf->rsize += rsize3;

    return rsize3;
}