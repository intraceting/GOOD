/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "tar.h"

/**
 * TAR的块长度(512Bytes)。
*/
#define GOOD_TAR_BLOCK_SIZE     512

/**
 * TAR头部信息.
 * 
 * 512 Bytes.
 */
typedef struct _good_tar_hdr
{
    char name[100];         /*   0 Dateiname*/
    char mode[8];           /* 100 Zugriffsrechte*/
    char uid[8];            /* 108 Benutzernummer*/
    char gid[8];            /* 116 Benutzergruppe*/
    char size[12];          /* 124 Dateigroesze*/
    char mtime[12];         /* 136 Zeit d. letzten Aenderung*/
    char chksum[8];         /* 148 Checksumme*/
    char typeflag;          /* 156 Typ der Datei*/
    char linkname[100];     /* 157 Zielname des Links*/
    char magic[TMAGLEN];    /* 257 "ustar"*/
    char version[TVERSLEN]; /* 263 Version v. star*/
    char uname[32];         /* 265 Benutzername	*/
    char gname[32];         /* 297 Gruppenname*/
    char devmajor[8];       /* 329 Major bei Geraeten*/
    char devminor[8];       /* 337 Minor bei Geraeten*/
    char prefix[155];       /* 345 Prefix fuer t_name*/

    /*Other*/
    union
    {
        /**
         * This is the ustar (Posix 1003.1) header.
        */
        struct
        {
            char mfill[12]; /* 500 Filler bis 512*/
        } ustar;

    } other;

} good_tar_hdr;

/*
 * gnu tar extensions:
*/

/** long link magic.*/
#define GOOD_USTAR_LONGNAME_MAGIC "././@LongLink"
/** including NULL byte. */
#define GOOD_USTAR_LONGNAME_MAGIC_LEN 14
/** Identifies the NEXT file on the tape  as having a long linkname.*/
#define GOOD_USTAR_LONGLINK_TYPE 'K'
/** Identifies the NEXT file on the tape  as having a long name.*/
#define GOOD_USTAR_LONGNAME_TYPE 'L'

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

uint32_t _good_tar_hdr_calc_checksum(good_tar_hdr *hdr)
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

uint32_t _good_tar_hdr_get_checksum(good_tar_hdr* hdr)
{
    uint32_t sum = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->chksum, sizeof (hdr->chksum));

    sscanf(buf, "%o", &sum);

    return sum;
}

int64_t _good_tar_hdr_get_size(good_tar_hdr* hdr)
{
    uint64_t size = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->size, sizeof (hdr->size));

    if (DecodeUint8(&size, buf, sizeof(hdr->size)) == 0)
        size = -1;

    return size;
}

time_t _good_tar_hdr_get_mtime(good_tar_hdr *hdr)
{
    time_t mtime = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->mtime, sizeof (hdr->mtime));

    sscanf(buf, "%lo", &mtime);

    return mtime;
}

uint32_t _good_tar_hdr_get_mode(good_tar_hdr *hdr)
{
    uint32_t mode = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->mode, sizeof (hdr->mode));
    sscanf(buf, "%o", &mode);

    return mode;
}

void _good_tar_hdr_fill(good_tar_hdr *hdr,char typeflag,
                       const char name[100],const char linkname[100],
                       int64_t size, time_t time, uint32_t mode)
{
    assert(hdr != NULL && name != NULL);
    assert(size >= 0);
    assert(name[0] != '\0');

    /**/
    hdr->typeflag = typeflag;

    /*Max 99 bytes.*/
    strncpy(hdr->name, name, sizeof (hdr->name) - 1);

    /*Max 99 bytes, may be NULL(0).*/
    if(linkname)
        strncpy(hdr->linkname, linkname, sizeof (hdr->linkname) - 1);

    strncpy(hdr->magic, TMAGIC, TMAGLEN);
    strncpy(hdr->version, TVERSION, TVERSLEN);

    snprintf(hdr->mode, 8, "%07o", (mode & (S_IRWXU | S_IRWXG | S_IRWXO)));

    snprintf(hdr->mtime, 12, "%011lo", time);

    /*max 8GB*/
    if (size < 1024 * 1024 * 1024 * 8LL)
        snprintf(hdr->size, 12, "%011lo", size); //must be 11
    else
        EncodeUint8(size, hdr->size, 11); //must be 11
    
    /**/
    snprintf(hdr->chksum, 8, "%07ou", good_tar_hdr_calc_checksum(hdr));
}

int _good_tar_hdr_verify(good_tar_hdr *hdr)
{
    uint32_t old_sum = 0;
    uint32_t now_sum = 0;

    assert(hdr != NULL);

    if(good_strncmp(hdr->magic, TMAGIC, strlen(TMAGIC),1) != 0)
        return 0;

    old_sum = good_tar_hdr_get_checksum(hdr);
    now_sum = good_tar_hdr_calc_checksum(hdr);

    if(old_sum != now_sum)
        return 0;

    return 1;
}

ssize_t good_tar_read(good_tar_t *tar, void *data, size_t size)
{
    ssize_t rsize = 0;
    size_t rsize2 = 0;

    assert(tar != NULL && data != NULL && size > 0);
    assert(tar->fd >= 0);

    if (tar->buf)
    {
        assert(tar->buf->data != NULL && tar->buf->size > 0);

        while (rsize < size)
        {
            if (tar->buf->wsize > 0)
            {
                /*缓存有数据，先从缓存读取。*/
                rsize2 = good_buffer_read(tar->buf, GOOD_PTR2PTR(void, data, rsize), size - rsize);
                if (rsize2 <= 0)
                    break;

                /*累加读取长度。*/
                rsize += rsize2;

                /*吸收已经读取的缓存数据。*/
                good_buffer_drain(tar->buf);
            }
            else if (tar->buf->wsize == 0 || (size - rsize) < tar->buf->size)
            {
                /*
                 * 满足以下两个条件之一，则先读取到缓存空间。
                 * 1：缓存无数据。
                 * 2：待读数据小于缓存空间。
                */
                rsize2 = good_buffer_import_atmost(tar->buf, tar->fd, tar->buf->size);
                if (rsize2 <= 0)
                    break;
            }
            else
            {
                assert((size - rsize) >= tar->buf->size);

                /*
                 * 同时满足以下两个条件，把数据直接从文件中读取。
                 * 1：缓存无数据。
                 * 2：待读取数据大于缓存空间。
                */
                rsize2 = good_read(tar->fd, GOOD_PTR2PTR(void, data, rsize), tar->buf->size);
                if (rsize2 <= 0)
                    break;

                /*累加读取长度。*/
                rsize += rsize2;

                /*带缓存的读取，每次必须读取相同的大小。*/
                if (rsize2 != tar->buf->size)
                    break;
            }
        }
    }
    else
    {
        /*无缓存空间，直接从文件读取。*/
        rsize = good_read(tar->fd, data, size);
    }

    return rsize;
}

ssize_t good_tar_write(good_tar_t *tar, const void *data, size_t size)
{
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(tar != NULL && data != NULL && size > 0);
    assert(tar->fd >= 0);

    if (tar->buf)
    {
        assert(tar->buf->data != NULL && tar->buf->size > 0);

        while (wsize < size)
        {
            if (tar->buf->wsize > 0 && tar->buf->wsize == tar->buf->size)
            {
                /*缓存空间已满，先把缓存数据导出到文件。*/
                wsize2 = good_buffer_export_atmost(tar->buf, tar->fd, tar->buf->size);
                if (wsize2 <= 0)
                    break;

                /*吸收已经导出(已经写入到文件)的缓存数据。*/
                good_buffer_drain(tar->buf);
            }
            else if (tar->buf->wsize > 0 || (size - wsize) < tar->buf->size)
            {
                /* 
                 * 满足以下两个条件之一，则先把数据写进缓存空间。
                 * 1：缓存有数据，但未满。
                 * 2：缓存无数据，但是待写入数据小于缓存空间。
                 */
                wsize2 = good_buffer_write(tar->buf, GOOD_PTR2PTR(void, data, wsize), size - wsize);
                if (wsize2 <= 0)
                    break;

                /*累加写入长度。*/
                wsize += wsize2;
            }
            else
            {
                assert((size - wsize) >= tar->buf->size);

                /*
                 * 同时满足以下两个条件，把数据直接写入到文件。
                 * 1：缓存无数据。
                 * 2：待写入数据大于缓存空间。
                */
                wsize2 = good_write(tar->fd, GOOD_PTR2PTR(void, data, wsize), tar->buf->size);
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
        wsize = good_write(tar->fd, data, size);
    }

    return wsize;
}

int good_tar_write_trailer(good_tar_t *tar, uint8_t stuffing)
{
    ssize_t wsize2 = 0;

    assert(tar != NULL);
    assert(tar->fd >= 0);

    /*无缓存。*/
    if (!tar->buf)
        return 0;

    assert(tar->buf->data != NULL && tar->buf->size > 0);

    /*缓存无数据。*/
    if (tar->buf->wsize == 0)
        return 0;

    /*缓存有数据，先用填充物填满缓存空间。*/
    good_buffer_fill(tar->buf, stuffing);

    /*把缓存数据导出到文件。*/
    wsize2 = good_buffer_export_atmost(tar->buf, tar->fd, tar->buf->size);
    if (wsize2 <= 0)
        return -1;

    /*吸收已经导出(已经写入到文件)的缓存数据。*/
    good_buffer_drain(tar->buf);

    /*检查是否有数据未导出。*/
    if (tar->buf->wsize == 0)
        return 0;

    return -1;
}

int good_tar_write_hdr(good_tar_t *tar, const char *name, const struct stat *attr, const char *linkname)
{
    int namelen;
    int linknamelen;
    int longname_space;
    int longlink_space;
    good_tar_hdr *hdr;
    char *longname;
    char *longlink;
    void *tmp;
    int chk;

    assert(tar != NULL && name != NULL && attr != NULL);

    assert(tar->fd >= 0);
    assert(name[0] != '\0');
    assert(S_ISREG(attr->st_mode) || S_ISDIR(attr->st_mode) || S_ISLNK(attr->st_mode));

    hdr = (good_tar_hdr *)good_heap_alloc(GOOD_TAR_BLOCK_SIZE);
    if (hdr == NULL)
        goto final_error;

    /**/
    if(linkname != NULL && linkname[0] != '\0')
    {
        /*计算链接名的长度。*/
        linknamelen = strlen(linkname);

        /*链接名的长度大于或等于100时，需要特别处理。*/
        if (linknamelen >= 100)
        {
            /*计算长链接名占用的空间。*/
            longlink_space = good_align(linknamelen, GOOD_TAR_BLOCK_SIZE);
            longlink = (char *)good_heap_alloc(longlink_space);
            if (longlink == NULL)
                goto final_error;

            
        }


    }

    /*计算文件名的长度。*/
    namelen = strlen(name);

    /*文件名(包括路径)的长度大于或等于100时，需要特别处理。*/
    if (namelen >= 100)
    {   
        /*计算长文件名占用的空间。*/
        longname_space = good_align(namelen,GOOD_TAR_BLOCK_SIZE);
        longname = (char*)good_heap_alloc(longname_space);
        if (longname == NULL)
            goto final_error;

        /*复制长文件名。*/
        strncpy(longname,name,namelen);

        /*填充长文件名的头部信息。*/
        good_tar_hdr_fill(hdr, GOOD_USTAR_LONGNAME_TYPE, GOOD_USTAR_LONGNAME_MAGIC,namelen, 0, 0);

        /*长文件名的头部写入到文件。*/
        chk = ((good_tar_write(tar, hdr, GOOD_TAR_BLOCK_SIZE) == GOOD_TAR_BLOCK_SIZE) ? 0 : -1);
        if (chk != 0)
            goto final_error;

        /*长文件名写入到文件。*/
        chk = ((good_tar_write(tar, longname, longname_space) == longname_space) ? 0 : -1);
        if (chk != 0)
            goto final_error;

        /*清空头部准备复用。*/
        memset(hdr,0,GOOD_TAR_BLOCK_SIZE);
    }

    /*填充头部信息。*/
    if (S_ISREG(attr->st_mode))
        good_tar_hdr_fill(hdr, REGTYPE, name, "", attr->st_size, attr->st_mtim.tv_sec, attr->st_mode);
    else if (S_ISDIR(attr->st_mode))
        good_tar_hdr_fill(hdr, DIRTYPE, name, "",0, attr->st_mtim.tv_sec, attr->st_mode);

    /*写入到文件。*/
    chk = ((good_tar_write(tar, hdr, GOOD_TAR_BLOCK_SIZE) == GOOD_TAR_BLOCK_SIZE) ? 0 : -1);
    if (chk == 0)
        goto final;


final_error:

    /*出错了。*/
    chk = -1;

final:

    good_heap_freep((void**)&hdr);
    good_heap_freep((void**)&longname);
    good_heap_freep((void**)&longlink);

    return chk;
}

/*------------------------------------------------------------------------------------------------*/
