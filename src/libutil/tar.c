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

uint32_t good_tar_hdr_get_checksum(good_tar_hdr *hdr)
{
    uint32_t sum = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->chksum, sizeof(hdr->chksum));

    sscanf(buf, "%o", &sum);

    return sum;
}

int64_t good_tar_hdr_get_size(good_tar_hdr *hdr)
{
    uint64_t size = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->size, sizeof(hdr->size));

    if (DecodeUint8(&size, buf, sizeof(hdr->size)) == 0)
        size = -1;

    return size;
}

time_t good_tar_hdr_get_mtime(good_tar_hdr *hdr)
{
    time_t mtime = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->mtime, sizeof(hdr->mtime));

    sscanf(buf, "%lo", &mtime);

    return mtime;
}

mode_t good_tar_hdr_get_mode(good_tar_hdr *hdr)
{
    mode_t mode = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->mode, sizeof(hdr->mode));
    sscanf(buf, "%o", &mode);

    return mode;
}

uid_t good_tar_hdr_get_uid(good_tar_hdr *hdr)
{
    uid_t uid = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->uid, sizeof(hdr->uid));
    sscanf(buf, "%o", &uid);

    return uid;
}

gid_t good_tar_hdr_get_gid(good_tar_hdr *hdr)
{
    gid_t gid = 0;
    char buf[64] = {0};

    assert(hdr != NULL);

    memcpy(buf, hdr->gid, sizeof(hdr->gid));
    sscanf(buf, "%o", &gid);

    return gid;
}

void good_tar_hdr_fill(good_tar_hdr *hdr, char typeflag,
                       const char name[100], const char linkname[100],
                       int64_t size, time_t time, mode_t mode)
{
    assert(hdr != NULL && name != NULL);
    assert(size >= 0);
    assert(name[0] != '\0');

    /**/
    hdr->typeflag = typeflag;

    /*Max 99 bytes.*/
    strncpy(hdr->name, name, sizeof(hdr->name) - 1);

    /*Max 99 bytes, may be NULL(0).*/
    if (linkname)
        strncpy(hdr->linkname, linkname, sizeof(hdr->linkname) - 1);

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

int good_tar_hdr_verify(good_tar_hdr *hdr)
{
    uint32_t old_sum = 0;
    uint32_t now_sum = 0;

    assert(hdr != NULL);

    if (good_strncmp(hdr->magic, TMAGIC, strlen(TMAGIC), 1) != 0)
        return 0;

    old_sum = good_tar_hdr_get_checksum(hdr);
    now_sum = good_tar_hdr_calc_checksum(hdr);

    if (old_sum != now_sum)
        return 0;

    return 1;
}

ssize_t good_tar_read(good_tar_t *tar, void *data, size_t size)
{
    assert(tar != NULL && data != NULL && size > 0);

    return good_block_read(tar->fd, data, size, tar->buf);
}

int good_tar_read_align(good_tar_t *tar, size_t size)
{
    char tmp[GOOD_TAR_BLOCK_SIZE] = {0};
    size_t fix_size;

    assert(tar != NULL && size > 0);

    /*计算需要读取对齐的差额长度。*/
    fix_size = good_align(size, GOOD_TAR_BLOCK_SIZE) - size;

    /*也许已经对齐。*/
    if (fix_size <= 0)
        return 0;

    return ((good_tar_read(tar, tmp, fix_size) == fix_size) ? 0 : -1);
}

ssize_t good_tar_write(good_tar_t *tar, const void *data, size_t size)
{
    assert(tar != NULL && data != NULL && size > 0);

    return good_block_write(tar->fd, data, size, tar->buf);
}

int good_tar_write_align(good_tar_t *tar, size_t size)
{
    char tmp[GOOD_TAR_BLOCK_SIZE] = {0};
    size_t fix_size;

    assert(tar != NULL && size > 0);

    /*计算需要写入对齐的差额长度。*/
    fix_size = good_align(size, GOOD_TAR_BLOCK_SIZE) - size;

    /*也许已经对齐。*/
    if (fix_size <= 0)
        return 0;

    return ((good_tar_write(tar, tmp, fix_size) == fix_size) ? 0 : -1);
}

int good_tar_write_trailer(good_tar_t *tar, uint8_t stuffing)
{
    assert(tar != NULL);

    return good_block_write_trailer(tar->fd, 0, tar->buf);
}

int good_tar_read_hdr(good_tar_t *tar, char name[PATH_MAX], struct stat *attr, char linkname[PATH_MAX])
{
    good_tar_hdr hdr = {0};
    int namelen = 0;
    int linknamelen = 0;

    assert(tar != NULL && name != NULL &&  attr != NULL && linkname != NULL);

    assert(tar->fd >= 0);

    /*完整的头部可能由多个组成。*/

again:
    
    if(good_tar_read(tar,&hdr,GOOD_TAR_BLOCK_SIZE) != GOOD_TAR_BLOCK_SIZE)
        goto final_error;

    if(!good_tar_hdr_verify(&hdr))
        goto final_error;
    
    if(hdr.typeflag == GOOD_USTAR_LONGLINK_TYPE)
    {
        /*长链接名需要特殊处理。*/

        if (good_strncmp(hdr.name, GOOD_USTAR_LONGNAME_MAGIC, GOOD_USTAR_LONGNAME_MAGIC_LEN, 1) != 0)
            goto final_error;

        linknamelen = good_tar_hdr_get_size(&hdr);
        if (linknamelen <= 0)
            goto final_error;

        if (good_tar_read(tar, linkname, linknamelen) != linknamelen)
            goto final_error;

        if (good_tar_read_align(tar, linknamelen) != 0)
            goto final_error;

        /*头部信息还不完整，继续读取。*/
        goto again;

    }
    else if(hdr.typeflag == GOOD_USTAR_LONGNAME_TYPE)
    {
        /*长文件名需要特殊处理。*/

        if (good_strncmp(hdr.name, GOOD_USTAR_LONGNAME_MAGIC, GOOD_USTAR_LONGNAME_MAGIC_LEN, 1) != 0)
            goto final_error;

        namelen = good_tar_hdr_get_size(&hdr);
        if (namelen <= 0)
            goto final_error;

        if (good_tar_read(tar, name, namelen) != namelen)
            goto final_error;

        if (good_tar_read_align(tar, namelen) != 0)
            goto final_error;

        /*头部信息还不完整，继续读取。*/
        goto again;
    }
    else
    {
        if (REGTYPE == hdr.typeflag || AREGTYPE == hdr.typeflag)
            attr->st_mode = __S_IFREG;
        else if (SYMTYPE == hdr.typeflag)
            attr->st_mode = __S_IFLNK;
        else if (DIRTYPE == hdr.typeflag)
            attr->st_mode = __S_IFDIR;
        else if (CHRTYPE == hdr.typeflag)
            attr->st_mode = __S_IFCHR;
        else if (BLKTYPE == hdr.typeflag)
            attr->st_mode = __S_IFBLK;
        else if (FIFOTYPE == hdr.typeflag)
            attr->st_mode = __S_IFIFO;
        else
            goto final_error;

        attr->st_size = good_tar_hdr_get_size(&hdr);
        attr->st_mode |= good_tar_hdr_get_mode(&hdr);
        attr->st_mtim.tv_sec = good_tar_hdr_get_mtime(&hdr);
        attr->st_gid = good_tar_hdr_get_gid(&hdr);
        attr->st_uid = good_tar_hdr_get_uid(&hdr);

        if (namelen <= 100)
            strncpy(name,hdr.name,GOOD_MIN(strlen(hdr.name),sizeof(hdr.name)));
        if (linknamelen <= 100)
            strncpy(linkname,hdr.linkname,GOOD_MIN(strlen(hdr.linkname),sizeof(hdr.linkname)));
    }


    return 0;

final_error:

    return -1;
}

int good_tar_write_hdr(good_tar_t *tar, const char *name, const struct stat *attr, const char *linkname)
{
    good_tar_hdr hdr = {0};
    int namelen = 0;
    int linknamelen = 0;
    int chk;

    assert(tar != NULL && name != NULL && attr != NULL);

    assert(tar->fd >= 0);
    assert(name[0] != '\0');
    assert(S_ISREG(attr->st_mode) || S_ISDIR(attr->st_mode) || S_ISLNK(attr->st_mode));

    /*计算文件名、链接名的长度。*/
    namelen = strlen(name);
    linknamelen = strlen(linkname);

    /*链接名的长度大于或等于100时，需要特别处理。*/
    if (linknamelen >= 100)
    {
        /*清空头部准备复用。*/
        memset(&hdr, 0, GOOD_TAR_BLOCK_SIZE);

        /*填充长链接名的头部信息。*/
        good_tar_hdr_fill(&hdr, GOOD_USTAR_LONGLINK_TYPE, GOOD_USTAR_LONGNAME_MAGIC, NULL, linknamelen, 0, 0);

        /*长链接名的头部写入到文件。*/
        if (good_tar_write(tar, &hdr, GOOD_TAR_BLOCK_SIZE) != GOOD_TAR_BLOCK_SIZE)
            goto final_error;

        /*长链接名写入到文件。*/
        if (good_tar_write(tar, linkname, linknamelen) != linknamelen)
            goto final_error;

        /*也许需要写入对齐。*/
        if (good_tar_write_align(tar, linknamelen) != 0)
            goto final_error;
    }

    /*文件名(包括路径)的长度大于或等于100时，需要特别处理。*/
    if (namelen >= 100)
    {
        /*清空头部准备复用。*/
        memset(&hdr, 0, GOOD_TAR_BLOCK_SIZE);

        /*填充长文件名的头部信息。*/
        good_tar_hdr_fill(&hdr, GOOD_USTAR_LONGNAME_TYPE, GOOD_USTAR_LONGNAME_MAGIC, NULL, namelen, 0, 0);

        /*长文件名的头部写入到文件。*/
        if (good_tar_write(tar, &hdr, GOOD_TAR_BLOCK_SIZE) != GOOD_TAR_BLOCK_SIZE)
            goto final_error;

        /*长文件名写入到文件。*/
        if (good_tar_write(tar, name, namelen) != namelen)
            goto final_error;

        /*也许需要写入对齐。*/
        if (good_tar_write_align(tar, namelen) != 0)
            goto final_error;
    }

    /*清空头部准备复用。*/
    memset(&hdr, 0, GOOD_TAR_BLOCK_SIZE);

    /*填充头部信息。*/
    if (S_ISREG(attr->st_mode))
        good_tar_hdr_fill(&hdr, REGTYPE, name, NULL, attr->st_size, attr->st_mtim.tv_sec, attr->st_mode);
    else if (S_ISDIR(attr->st_mode))
        good_tar_hdr_fill(&hdr, DIRTYPE, name, NULL, 0, attr->st_mtim.tv_sec, attr->st_mode);
    else if (S_ISLNK(attr->st_mode))
        good_tar_hdr_fill(&hdr, SYMTYPE, name, linkname, 0, attr->st_mtim.tv_sec, attr->st_mode);

    /*写入到文件。*/
    if (good_tar_write(tar, &hdr, GOOD_TAR_BLOCK_SIZE) != GOOD_TAR_BLOCK_SIZE)
        goto final_error;

    return 0;

final_error:

    return -1;
}
