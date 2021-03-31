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
     * 内存块。
     *
     * @note 尽量不要直接访问。
    */
    good_allocator_t *alloc;

    /**
     * 缓存指针。
    */
    void* data;

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
 * 申请缓存。
 * 
 * @param size 缓存大小。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_heap_alloc()
 * @see good_heap_freep()
 * @see good_allocator_alloc2()
 * 
 */
good_buffer_t *good_buffer_alloc(size_t size);

/**
 * 缓存释放。
 * 
 * @param dst 缓存指针的指针。函数返回前修改为NULL(0);
 * 
 * @see good_heap_freep()
 * @see good_allocator_unref()
*/
void good_buffer_freep(good_buffer_t **dst);

/**
 * 缓存复制。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_heap_alloc()
 * @see good_buffer_alloc()
 * @see good_allocator_refer()
 * @see memcpy()
 */
good_buffer_t *good_buffer_copy(good_buffer_t *src);

/**
 * 缓存克隆。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_buffer_alloc()
 * @see memcpy()
 */
good_buffer_t *good_buffer_clone(good_buffer_t *src);

/**
 * 缓存私有化。
 * 
 * @return 0 成功，-1 失败。
 * 
 * @see good_allocator_privatize()
*/
int good_buffer_privatize(good_buffer_t *dst);

/**
 * 格式化输出，追加写入缓存。
 * 
 * @return > 0 写入长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_vprintf(good_buffer_t *buf,const char * fmt, va_list args);

/**
 * 格式化输出，追加写入缓存。
 * 
 * @return 写入长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_printf(good_buffer_t *buf,const char * fmt,...);

/**
 * 追加写入缓存。
 * 
 * @return 写入长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see memcpy();
*/
ssize_t good_buffer_write(good_buffer_t *buf, const void *data, size_t size);

/**
 * 追加填满缓存。
 * 
 * @param stuffing 填充物。
 * @return 写入长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_fill(good_buffer_t *buf,uint8_t stuffing);

/**
 * 吸收已读数据，未读数据移动到缓存首地址。
*/
void good_buffer_read_vacuum(good_buffer_t *buf);



#endif //GOOD_UTIL_BUFFER_H