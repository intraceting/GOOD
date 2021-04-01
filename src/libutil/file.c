/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "file.h"

ssize_t good_write(int fd, const void *data, size_t size, good_buffer_t *buf)
{
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(fd >= 0 && data && size > 0);

    if (buf)
    {
        assert(buf->number >= 1 && buf->data[0] && buf->size[0] > 0);

        while (wsize < size)
        {
            if (buf->size1[0] > 0 && buf->size1[0] == buf->size[0])
            {
                /*
                 * 缓存满了，需要执行落盘操作。
                */
                wsize2 = good_write(fd, buf->data[0], buf->size1[0], NULL);
                if (wsize2 != buf->size1[0])
                {
                    /*
                     * 移动剩余数据到开缓存开头。
                    */
                    if (wsize2 > 0)
                    {
                        buf->size1[0] -= wsize2;
                        memmove(buf->data[0], GOOD_PTR2PTR(void, buf->data[0], wsize2), buf->size1[0]);
                    }

                    /*
                     * 数据未写完，提前中断。
                    */
                    break;
                }
                else
                {
                    buf->size1[0] = 0;
                }
            }
            else
            {
                if (buf->size1[0] == 0 && (size - wsize) >= buf->size[0])
                {
                    /*
                     * 缓存是空的，并且待写数据大于缓存空间，则直接落盘。
                    */
                    wsize2 = good_write(fd, GOOD_PTR2PTR(void, data, wsize), buf->size[0], NULL);
                    if (wsize2 == buf->size[0])
                    {
                        wsize += wsize2;
                    }
                    else
                    {
                        if (wsize2 > 0)
                            wsize += wsize2;

                        /*
                         * 数据未写完，提前中断。
                        */
                        break;
                    }
                }
                else
                {
                    /*
                     * 缓存有数据，或待写数据小于缓存空间，则先写入缓存。
                    */
                    wsize2 = GOOD_MIN(size - wsize, buf->size[0] - buf->size1[0]);
                    memcpy(GOOD_PTR2PTR(void, buf->data[0], buf->size1[0]), GOOD_PTR2PTR(void, data, wsize), wsize2);

                    wsize += wsize2;
                    buf->size1[0] += wsize2;
                }
            }
        }
    }
    else
    {
        /*
         * 落盘。
        */
        wsize = write(fd, data, size);
        if (wsize > 0)
        {
            if (wsize < size)
            {
                /*
                 * 有的系统超过2GB，需要分段落盘。
                */
                wsize2 = good_write(fd, GOOD_PTR2PTR(void, data, wsize), size - wsize, NULL);
                if (wsize2 > 0)
                    wsize += wsize2;
            }
        }
    }

    return wsize;
}

ssize_t good_write_trailer(int fd, int fill, uint8_t stuffing, good_buffer_t *buf)
{
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    if (buf)
    {
        assert(buf->number >= 1 && buf->data[0] && buf->size[0] > 0);

        if (buf->size1[0] > 0)
        {
            /*
             * 可能需要把缓存填满了。
            */
            if (fill)
            {
                memset(GOOD_PTR2PTR(void, buf->data[0], buf->size1[0]), stuffing, buf->size[0] - buf->size1[0]);
                buf->size1[0] = buf->size[0];
            }

            wsize2 = good_write(fd, buf->data[0], buf->size1[0], NULL);
            if (wsize2 == buf->size1[0])
            {
                wsize = buf->size1[0] = buf->size[0] = 0;
            }
            else if (wsize2 > 0)
            {
                buf->size1[0] -= wsize2;
                memmove(buf->data[0], GOOD_PTR2PTR(void, buf->data[0], wsize2), buf->size1[0]);
            }
            else
            {
                wsize = buf->size[0];
            }
        }
        else
        {
            wsize = 0;
        }
    }
    else
    {
        wsize = 0;
    }

    return wsize;
}

ssize_t good_read(int fd, void *data, size_t size, good_buffer_t *buf)
{
    ssize_t rsize = 0;
    ssize_t rsize2 = 0;

    assert(fd >= 0 && data && size > 0);

    if (buf)
    {
        assert(buf->number >= 1 && buf->data[0] && buf->size[0] > 0);

        while (rsize < size)
        {
            if (buf->size1[0] < buf->size2[0])
            {
                /*
                 * 缓存中有未读数据，先从缓存中读取。
                */
                rsize2 = GOOD_MIN(buf->size2[0] - buf->size1[0], size - rsize);
                memcpy(GOOD_PTR2PTR(void, data, rsize), GOOD_PTR2PTR(void, buf->data[0], buf->size1[0]), rsize2);

                rsize += rsize2;
                buf->size1[0] += rsize2;
            }
            else
            {
                /*
                 * 清空缓存。
                */
                buf->size1[0] = buf->size2[0] = 0;

                /*
                 * 读取数据，填充到缓存区。
                */
                rsize2 = good_read(fd, buf->data[0], buf->size[0], NULL);
                if (rsize2 > 0)
                    buf->size2[0] = rsize2;

                /*
                 * 无数据，中断读取。
                */
                if (rsize2 <= 0)
                    break;
            }
        }
    }
    else
    {
        /*
         * 读取
        */
        rsize = read(fd, data, size);
        if (rsize > 0)
        {
            if (rsize < size)
            {
                /*
                * 有的系统超过2GB，需要分段读取。
                */
                rsize2 = good_read(fd, GOOD_PTR2PTR(char, data, rsize), size - rsize, NULL);
                if (rsize2 > 0)
                    rsize += rsize2;
            }
        }
    }

    return rsize;
}
