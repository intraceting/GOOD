/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_TAR_H
#define GOOD_UTIL_TAR_H

#include "general.h"
#include "buffer.h"

/**
 * This is the tar header.
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
 * 从TAR文件读取数据。
 * 
 * @param buf NULL(0) 自由块大小，!NULL(0) 定长块大小。
 * 
 * @return > 0 读取的长度，<= 0 读取失败或已到末尾。
*/
ssize_t good_tar_read(int fd, void *data, size_t size, good_buffer_t *buf);

/**
 * 向TAR文件写入数据。
 * 
 * @param buf NULL(0) 自由块大小，!NULL(0) 定长块大小。
 * 
 * @return > 0 写入的长度，<= 0 写入失败或空间不足。
*/
ssize_t good_tar_write(int fd, const void *data, size_t size, good_buffer_t *buf);

/**
 * 向TAR文件文件写入补齐数据。
 * 
 * @param buf NULL(0) 自由块大小，!NULL(0) 定长块大小。
 * 
 * @param stuffing 填充物。
 * 
 * @return > 0 缓存数据全部写入到文件，= 0 缓存无数据或无缓存，< 0 写入失败或空间不足(剩余数据在缓存中)。
*/
int good_tar_write_trailer(int fd, uint8_t stuffing, good_buffer_t *buf);

/**
 * 计算TAR头部校验和。
*/
uint32_t good_tar_hdr_calc_checksum(good_tar_hdr *hdr);

/** 
 * 提取TAR头部中的校验和字段。
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
uint32_t good_tar_hdr_get_mode(good_tar_hdr *hdr);

#endif //GOOD_UTIL_TAR_H