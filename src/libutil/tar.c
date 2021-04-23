/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "tar.h"

/*
 * 以下几个方法的来源：http://www.ncbi.nlm.nih.gov/IEB/ToolBox/CPP_DOC/lxr/source/src/util/compress/api/tar.cpp 
*/

/** 
 * Convert a number to an octal string padded to the left
 * with [leading] zeros ('0') and having _no_ trailing '\0'.
*/
static int NumToOctal(uint64_t val, char *ptr, size_t len)
{
    assert(len > 0);

    do
    {
        ptr[--len] = '0' + (char)(val & 7);
        val >>= 3;
    } while (len);

    return val ? 0 : 1;
}

/** 
 * Convert an octal number (possibly preceded by spaces) to numeric form.
 *  Stop either at the end of the field or at first '\0' (if any).
*/
static int OctalToNum(uint64_t* val, const char* ptr, size_t len)
{
    assert(len > 0);

    size_t i = *ptr ? 0 : 1;
    while (i < len && ptr[i])
    {
        if (!isspace((unsigned char) ptr[i]))
            break;
        i++;
    }
    *val = 0;
    int okay = 0;
    while (i < len && ptr[i] >= '0' && ptr[i] <= '7')
    {
        okay = 1;
        *val <<= 3;
        *val |= ptr[i++] - '0';
    }
    while (i < len && ptr[i])
    {
        if (!isspace((unsigned char) ptr[i]))
            return 0;
        i++;
    }

    return okay;
}

/**
 *
*/
static int NumToBase256(uint64_t val, char* ptr, size_t len)
{
    assert(len > 0);

    do
    {
        ptr[--len] = (unsigned char) (val & 0xFF);
        val >>= 8;
    }
    while (len);

    *ptr |= '\x80'; // set base-256 encoding flag

    return val ? 0 : 1;
}

/**
 * Return 0 (false) if conversion failed; 1 if the value converted to
 * conventional octal representation (perhaps, with terminating '\0'
 * sacrificed), or -1 if the value converted using base-256.
*/
static int EncodeUint8(uint64_t val, char* ptr, size_t len)
{
    // Max file size (for len == 12):
    if (NumToOctal(val, ptr, len))
    {
        //   8GiB-1         
        return 1/*okay*/;
    }
    if (NumToOctal(val, ptr, ++len))
    {
        //   64GiB-1
        return 1/*okay*/;
    }
    if (NumToBase256(val, ptr, len))
    {
        //   up to 2^94-1
        return -1/*okay, base-256*/;
    }
    return 0/*failure*/;
}

/**
 * Return true if conversion succeeded;  false otherwise.
*/ 
static int Base256ToNum(uint64_t* val, const char* ptr, size_t len)
{
    const uint64_t lim = UINT64_MAX >> 8;

    if (*ptr & '\x40')
    { 
        // negative base-256?
        return 0;
    }

    *val = *ptr++ & '\x3F';

    while (--len)
    {
        if (*val > lim)
        {
            return 0;
        }
        *val <<= 8;
        *val |= (unsigned char) (*ptr++);
    }

    return 1;
}

/**
 * Return 0 (false) if conversion failed; 1 if the value was read into
 * as a conventional octal string (perhaps, without the terminating '\0');
 * or -1 if base-256 representation used.
*/
static int DecodeUint8(uint64_t* val, const char* ptr, size_t len)
{
    if (*ptr & '\x80')
    {
        return Base256ToNum(val, ptr, len) ? -1/*okay*/ : 0/*failure*/;
    }
    else
    {
        return OctalToNum(val, ptr, len) ? 1/*okay*/ : 0/*failure*/;
    }
}

/*
 * 以上几个方法的来源：http://www.ncbi.nlm.nih.gov/IEB/ToolBox/CPP_DOC/lxr/source/src/util/compress/api/tar.cpp 
*/

ssize_t good_tar_read(int fd, void *data, size_t size, good_buffer_t *buf)
{
    ssize_t rsize = 0;
    size_t rsize2 = 0;

    assert(fd >= 0 && data != NULL && size > 0);

    if (buf)
    {
        assert(buf->data != NULL && buf->size > 0);

        while (rsize < size)
        {
            if (buf->wsize > 0)
            {
                /*缓存有数据，先从缓存读取。*/
                rsize2 = good_buffer_read(buf, GOOD_PTR2PTR(void, data, rsize), size - rsize);
                if (rsize2 <= 0)
                    break;

                /*累加读取长度。*/
                rsize += rsize2;

                /*吸收已经读取的缓存数据。*/
                good_buffer_drain(buf);
            }
            else if (buf->wsize == 0 || (size - rsize) < buf->size)
            {
                /*
                 * 满足以下两个条件之一，则先读取到缓存空间。
                 * 1：缓存无数据。
                 * 2：待读数据小于缓存空间。
                */
                rsize2 = good_buffer_import_atmost(buf, fd, buf->size);
                if (rsize2 <= 0)
                    break;
            }
            else
            {
                assert((size - rsize) >= buf->size);

                /*
                 * 同时满足以下两个条件，把数据直接从文件中读取。
                 * 1：缓存无数据。
                 * 2：待读取数据大于缓存空间。
                */
                rsize2 = good_read(fd, GOOD_PTR2PTR(void, data, rsize), buf->size);
                if (rsize2 <= 0)
                    break;

                /*累加读取长度。*/
                rsize += rsize2;

                /*带缓存的读取，每次必须读取相同的大小。*/
                if (rsize2 != buf->size)
                    break;
            }
        }
    }
    else
    {
        /*无缓存空间，直接从文件读取。*/
        rsize = good_read(fd, data, size);
    }

    return rsize;
}

ssize_t good_tar_write(int fd, const void *data, size_t size, good_buffer_t *buf)
{
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(fd >= 0 && data != NULL && size > 0);

    if (buf)
    {
        assert(buf->data != NULL && buf->size > 0);

        while (wsize < size)
        {
            if (buf->wsize > 0 && buf->wsize == buf->size)
            {
                /*缓存空间已满，先把缓存数据导出到文件。*/
                wsize2 = good_buffer_export_atmost(buf, fd, buf->size);
                if (wsize2 <= 0)
                    break;

                /*吸收已经导出(已经写入到文件)的缓存数据。*/
                good_buffer_drain(buf);
            }
            else if (buf->wsize > 0 || (size - wsize) < buf->size)
            {
                /* 
                 * 满足以下两个条件之一，则先把数据写进缓存空间。
                 * 1：缓存有数据，但未满。
                 * 2：缓存无数据，但是待写入数据小于缓存空间。
                 */
                wsize2 = good_buffer_write(buf, GOOD_PTR2PTR(void, data, wsize), size - wsize);
                if (wsize2 <= 0)
                    break;

                /*累加写入长度。*/
                wsize += wsize2;
            }
            else
            {
                assert((size - wsize) >= buf->size);

                /*
                 * 同时满足以下两个条件，把数据直接写入到文件。
                 * 1：缓存无数据。
                 * 2：待写入数据大于缓存空间。
                */
                wsize2 = good_write(fd, GOOD_PTR2PTR(void, data, wsize), buf->size);
                if (wsize2 <= 0)
                    break;

                /*累加写入长度。*/
                wsize += wsize2;
            }
        }
    }
    else
    {
        /*无缓存空间，直接写入文件。*/
        wsize = good_write(fd, data, size);
    }

    return wsize;
}

int good_tar_write_trailer(int fd, uint8_t stuffing, good_buffer_t *buf)
{
    ssize_t wsize2 = 0;

    assert(fd >= 0);

    /*无缓存。*/
    if (!buf)
        return 0;

    assert(buf->data != NULL && buf->size > 0);

    /*缓存无数据。*/
    if (buf->wsize == 0)
        return 0;

    /*缓存有数据，先用填充物填满缓存空间。*/
    good_buffer_fill(buf, stuffing);

    /*把缓存数据导出到文件。*/
    wsize2 = good_buffer_export_atmost(buf, fd, buf->size);
    if (wsize2 <= 0)
        return -1;

    /*吸收已经导出(已经写入到文件)的缓存数据。*/
    good_buffer_drain(buf);

    /*检查是否有数据未导出。*/
    if (buf->wsize == 0)
        return 0;

    return -1;
}

uint32_t good_tar_hdr_calc_checksum(good_tar_hdr *hdr)
{
    uint32_t sum = 0;
    int i = 0;

    assert(hdr != NULL);

    for (i = 0; i < 148; ++i)
    {
        sum += *GOOD_PTR2PTR(uint8_t, hdr, i);
    }

    /*Skip CHECKSUM column(8 bytes)*/
    for (i += 8; i < 512; ++i) //...........
    {
        sum += *GOOD_PTR2PTR(uint8_t, hdr, i);
    }

    sum += 256;

    return sum;
}

uint32_t good_tar_hdr_get_checksum(good_tar_hdr* hdr)
{
    uint32_t sum = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->chksum, sizeof (hdr->chksum));

    sscanf(buf, "%o", &sum);

    return sum;
}

int64_t good_tar_hdr_get_size(good_tar_hdr* hdr)
{
    uint32_t size = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->size, sizeof (hdr->size));

    if (DecodeUint8(&size, buf, sizeof(hdr->size)) == 0)
        size = -1;

    return size;
}

time_t good_tar_hdr_get_mtime(good_tar_hdr *hdr)
{
    time_t mtime = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->mtime, sizeof (hdr->mtime));

    sscanf(buf, "%lo", &mtime);

    return mtime;
}

uint32_t good_tar_hdr_get_mode(good_tar_hdr *hdr)
{
    uint32_t mode = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->mode, sizeof (hdr->mode));
    sscanf(buf, "%o", &mode);

    return mode;
}