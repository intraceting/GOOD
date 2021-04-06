/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "archive.h"

ssize_t good_archive_write(int fd, const void *data, size_t size, good_buffer_t *buf)
{
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(fd >= 0 && data && size > 0);

    if (buf)
    {
        assert(buf->data != NULL && buf->size > 0);

        while (wsize < size)
        {
            if (buf->wsize > 0 && buf->wsize == buf->size)
            {
                /*
                 * 缓存满了，需要执行落盘操作。
                */
                wsize2 = good_buffer_export(buf, fd);
                if (wsize2 <= 0)
                    break;
                    
                /*
                 * 排出缓存中已落盘的数据。
                */
                good_buffer_drain(buf);

                /*
                 * 无法全部落盘，可能空间不足，返回。
                */
                if (buf->rsize > 0)
                    break;
            }
            else
            {
                if (buf->wsize == 0 && (size - wsize) >= buf->size)
                {
                    /*
                     * 缓存是空的，并且待写数据大于缓存空间，则直接落盘。
                    */
                    wsize2 = good_archive_write(fd, GOOD_PTR2PTR(void, data, wsize), buf->size,NULL);
                    if (wsize2 == buf->size)
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
                    wsize2 = good_buffer_write(buf, GOOD_PTR2PTR(void, data, wsize), size - wsize);
                    if (wsize2 <= 0)
                        break;

                    wsize += wsize2;
                 }
            }
        }
    }
    else
    {
        /*
         * 落盘。
        */
        wsize = good_write(fd, data, size);
    }

    return wsize;
}

ssize_t good_archive_write_trailer(int fd, int fill, uint8_t stuffing, good_buffer_t *buf)
{
    ssize_t wremain = 0;
    ssize_t wsize2 = 0;

    if (buf)
    {
        assert(buf->data && buf->wsize > 0);

        if (buf->wsize > 0)
        {
            /*
             * 可能需要把缓存填满了。
            */
            if (fill)
                good_buffer_fill(buf,stuffing);

            /*
             * 导出数据到文件。
            */
            wsize2 = good_buffer_export(buf,fd);
            
            /*
            * 排出缓存中已落盘的数据。
            */
            good_buffer_drain(buf);

            /*
            * 如果无法全部落盘，可能空间不足。
            */
            wremain = buf->wsize;
        }
        else
        {
            wremain = 0;
        }
    }
    else
    {
        wremain = 0;
    }

    return wremain;
}

ssize_t good_archive_read(int fd, void *data, size_t size, good_buffer_t *buf)
{
    ssize_t rsize = 0;
    ssize_t rsize2 = 0;

    assert(fd >= 0 && data && size > 0);

    if (buf)
    {
        assert( buf->data != NULL && buf->wsize > 0);

        while (rsize < size)
        {
            if (buf->wsize > 0 && buf->rsize < buf->wsize)
            {
                /*
                 * 缓存中有未读数据，先从缓存中读取。
                */
                rsize2 = good_buffer_read(buf,GOOD_PTR2PTR(void, data, rsize),size - rsize);
                rsize += rsize2;

                /*
                * 排出缓存中已读取的数据。
                */
                good_buffer_drain(buf);
            }
            else
            {
                /*
                 * 导入数据到缓存区。
                */
                rsize2 = good_buffer_import(buf,fd);
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
        rsize = good_read(fd, data, size);
    }

    return rsize;
}
