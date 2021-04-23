/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "tar.h"

ssize_t good_tar_read(int fd, void *data, size_t size, good_buffer_t *buf)
{
    ssize_t rsize = -1;
    size_t rsize2 = 0;

    assert(fd >= 0 && data != NULL && size > 0);

    if(buf)
    {
        assert(buf->data != NULL && buf->size >0);

        while (rsize < size)
        {
            if (buf->wsize > 0)
            {
                /*缓存中有数据，先从缓存读取。*/
                rsize2 = good_buffer_read(buf, GOOD_PTR2PTR(void, data, rsize), size - rsize);
                if (rsize2 <= 0)
                    break;

                /*累加读取长度。*/
                rsize += rsize2;

                /*吸收已经读取的缓存数据。*/
                good_buffer_drain(buf);
            }
            else if (size - rsize > buf->size)
            {
                /*缓存为空，并且待读取数据大于缓存空间，直接从文件读取。*/
                rsize2 = good_read(fd,GOOD_PTR2PTR(void, data, rsize),buf->size);
                if (rsize2 <= 0)
                    break;

                /*累加读取长度。*/
                rsize += rsize2;

                /*带缓存的读取，每次必须读取相同的大小。*/
                if(rsize2 != buf->size)
                    break;
            }
            else
            {
                /*缓存为空，但是待读数据小于缓存空间，先读取到缓存空间。*/
                rsize2 = good_buffer_import_atmost(buf,fd,buf->size);
                if(rsize2<=0)
                    break;
            }
        }
    }
    else
    {
        /*无缓存空间，直接从文件读取。*/
        rsize = good_read(fd,data,size);
    }

    return rsize;
}