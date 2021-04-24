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
 * 从TAR文件读取数据。
 * 
 * @return > 0 读取的长度，<= 0 读取失败或已到末尾。
*/
ssize_t good_tar_read(good_tar_t *tar, void *data, size_t size);

/**
 * 向TAR文件写入数据。
 * 
 * @return > 0 写入的长度，<= 0 写入失败或空间不足。
*/
ssize_t good_tar_write(good_tar_t *tar, const void *data, size_t size);

/**
 * 向TAR文件文件写入补齐数据。
 * 
 * @param stuffing 填充物。
 * 
 * @return > 0 缓存数据全部写入到文件，= 0 缓存无数据或无缓存，< 0 写入失败或空间不足(剩余数据在缓存中)。
*/
int good_tar_write_trailer(good_tar_t *tar, uint8_t stuffing);

/**
 * 向文件写入TAR头部。
 * 
 * @param name 文件名的指针(包括路径)。
 * @param attr 属性的指针。
 * @param linkname 链接名的指针，可以为NULL(0)。
 * 
*/
int good_tar_write_hdr(good_tar_t *tar, const char *name, const struct stat *attr, const char *linkname);

#endif //GOOD_UTIL_TAR_H