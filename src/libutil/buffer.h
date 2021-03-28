/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_BUFFER_H
#define GOOD_UTIL_BUFFER_H

#include "general.h"
#include "allocator.h"

/**
 * 简单的缓存。
 * 
*/
typedef struct _good_buffer
{
    /**
     * 缓存指针。
    */
    uint8_t* data;

    /**
     * 容量大小。
    */
    size_t size;

    /**
     * 已读大小。
     * 
    */
    size_t rsize;

    /**
     * 已写大小。
    */
    size_t wsize;

} good_buffer_t;

/**
 * 格式化输出，追加写入缓存。
 * 
 * @return 写入长度(Bytes)。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_vprintf(good_buffer_t *buf,const char * fmt, va_list args);

/**
 * 格式化输出，追加写入缓存。
 * 
 * @return 写入长度(Bytes)。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_printf(good_buffer_t *buf,const char * fmt,...);

/**
 * 追加写入缓存。
 * 
 * @return 写入长度(Bytes)。
 * 
 * @see memcpy();
*/
ssize_t good_buffer_write(good_buffer_t *buf, const void *data, size_t size);

/**
 * 追加填满缓存。
 * 
 * @param stuffing 填充物。
 * @return 写入长度(Bytes)。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_fill(good_buffer_t *buf,uint8_t stuffing);

/**
 * 吸收已读数据，未读数据移动到缓存首地址。
*/
void good_buffer_vacuum(good_buffer_t *buf);



#endif //GOOD_UTIL_BUFFER_H