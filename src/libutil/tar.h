/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_TAR_H
#define GOOD_UTIL_TAR_H

#include "general.h"
#include "blockio.h"

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

/**
 * TAR
*/
typedef struct _good_tar
{
    /**
     * 文件句柄。
    */
    int fd;

    /**
     * 缓存。
     * 
     * NULL(0) 自由块大小，!NULL(0) 定长块大小。
    */
    good_buffer_t *buf;

} good_tar_t;


/**
 * 计算TAR头部较验和。
*/
uint32_t good_tar_hdr_calc_checksum(good_tar_hdr *hdr);

/** 
 * 提取TAR头部中的较验和字段。
*/
uint32_t good_tar_hdr_get_checksum(good_tar_hdr *hdr);

/** 
 * 提取TAR头部中的长度字段。
*/
int64_t good_tar_hdr_get_size(good_tar_hdr *hdr);

/** 
 * 提取TAR头部中的时间字段。
*/
time_t good_tar_hdr_get_mtime(good_tar_hdr *hdr);

/** 
 * 提取TAR头部中的状态字段。
*/
mode_t good_tar_hdr_get_mode(good_tar_hdr *hdr);

/** 
 * 提取TAR头部中的UID字段。
*/
uid_t good_tar_hdr_get_uid(good_tar_hdr *hdr);

/** 
 * 提取TAR头部中的GID字段。
*/
gid_t good_tar_hdr_get_gid(good_tar_hdr *hdr);

/** 
 * 填充TAR头部的字段。
 * 
 * @param name 文件名(包括路径)。including NULL byte。
 * @param linkname 链接名。including NULL byte。
*/
void good_tar_hdr_fill(good_tar_hdr *hdr,char typeflag,
                       const char name[100],const char linkname[100],
                       int64_t size, time_t time, mode_t mode);

/**
 * 较验TAR头部的较验和是否一致。
 * 
 * @return !0 一致，0 不一致。
*/
int good_tar_hdr_verify(good_tar_hdr *hdr);

/**
 * 从TAR文件中读数据。
 * 
 * @return > 0 读取的长度，<= 0 读取失败或已到末尾。
*/
ssize_t good_tar_read(good_tar_t *tar,void *data, size_t size);

/**
 * 从TAR文件中读数据对齐差额长度的数据，并丢弃掉。
 * 
 * @return 0 成功，-1 失败(读取失败或已到末尾)。
*/
int good_tar_read_align(good_tar_t *tar,size_t size);

/**
 * 向TAR文件中写数据。
 * 
 * @return > 0 写入的长度，<= 0 写入失败或空间不足。
*/
ssize_t good_tar_write(good_tar_t *tar,const void *data, size_t size);

/**
 * 向TAR文件中写数据对齐差额长度的数据。
 * 
 * @return 0 成功，-1 失败(写入失败或空间不足)。
*/
int good_tar_write_align(good_tar_t *tar,size_t size);

/**
 * 向TAR文件中以块为单位写补齐数据。
 * 
 * @param stuffing 填充物。
 * 
 * @return > 0 缓存数据全部写完，= 0 缓存无数据或无缓存，< 0 写入失败或空间不足(剩余数据在缓存中)。
*/
int good_tar_write_trailer(good_tar_t *tar, uint8_t stuffing);

/**
 * 从TAR文件中读数据TAR头部。
 * 
 * @param name 文件名的指针。
 * @param attr 属性的指针。
 * @param linkname 链接名的指针。
 * 
 * @return 0 成功，-1 失败(不是TAR格式或较验和错误)。
 * 
*/
int good_tar_read_hdr(good_tar_t *tar, char name[PATH_MAX], struct stat *attr, char linkname[PATH_MAX]);

/**
 * 向TAR文件写入TAR头部。
 * 
 * @param name 文件名的指针(包括路径)。
 * @param attr 属性的指针。
 * @param linkname 链接名的指针，可以为NULL(0)。
 * 
 * @return 0 成功，-1 失败(写入失败或空间不足)。
*/
int good_tar_write_hdr(good_tar_t *tar, const char *name, const struct stat *attr, const char *linkname);

#endif //GOOD_UTIL_TAR_H