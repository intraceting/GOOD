/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "buffer.h"

ssize_t good_buffer_vprintf(good_buffer_t *buf, const char *fmt, va_list args)
{
    ssize_t wsize2 = 0;

    assert(buf != NULL && buf->data != NULL && buf->size > 0);
    assert(fmt != NULL);

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

    assert(buf != NULL && buf->data != NULL && buf->size > 0);
    assert(fmt != NULL);

    va_list args;
    va_start(args, fmt);

    good_buffer_vprintf(buf, fmt, args);

    va_end(args);

    return wsize2;
}

ssize_t good_buffer_write(good_buffer_t *buf, const void *data, size_t size)
{
    ssize_t wsize2 = 0;

    assert(buf != NULL && buf->data != NULL && buf->size > 0);
    assert(data != NULL && size > 0);

    if (buf->wsize >= buf->size)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = GOOD_MIN(buf->size - buf->wsize, size);
    memcpy(GOOD_PTR2PTR(void, buf->data, buf->wsize), data, wsize2);
    buf->wsize += wsize2;

    return wsize2;
}

ssize_t good_buffer_fill(good_buffer_t *buf,uint8_t stuffing)
{
    ssize_t wsize2 = 0;

    assert(buf != NULL && buf->data != NULL && buf->size > 0);

    if (buf->wsize >= buf->size)
        GOOD_ERRNO_AND_RETURN1(ENOSPC, 0);

    wsize2 = buf->size - buf->wsize;
    memset(GOOD_PTR2PTR(void, buf->data, buf->wsize), stuffing, wsize2);
    buf->wsize += wsize2;

    return wsize2;
}

void good_buffer_vacuum(good_buffer_t *buf)
{
    assert(buf != NULL && buf->data != NULL && buf->size > 0);

    assert(buf->rsize <= buf->wsize);

    if (buf->rsize > 0)
    {
        buf->wsize -= buf->rsize;
        memmove(buf->data, GOOD_PTR2PTR(void, buf->data, buf->rsize), buf->wsize);
    }
}