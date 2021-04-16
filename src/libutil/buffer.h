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
     * 内存指针。
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
 * 创建。
 * 
 * @param alloc 内存块的指针。仅复制指针，不是指针对像引用。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_heap_alloc()
 * @see good_heap_freep()
 * 
 */
good_buffer_t *good_buffer_alloc(good_allocator_t *alloc);

/**
 * 创建。
 * 
 * @param size 容量(Bytes)。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_heap_alloc()
 * @see good_heap_freep()
 * @see good_allocator_alloc2()
 * 
 */
good_buffer_t *good_buffer_alloc2(size_t size);

/**
 * 释放。
 * 
 * @param dst 缓存指针的指针。函数返回前修改为NULL(0);
 * 
 * @see good_heap_freep()
 * @see good_allocator_unref()
*/
void good_buffer_freep(good_buffer_t **dst);

/**
 * 复制。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_heap_alloc()
 * @see good_buffer_alloc()
 * @see good_allocator_refer()
 */
good_buffer_t *good_buffer_copy(good_buffer_t *src);

/**
 * 克隆。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_buffer_alloc2()
 * @see memcpy()
 */
good_buffer_t *good_buffer_clone(good_buffer_t *src);

/**
 * 私有化。
 * 
 * 用于写前复制，或克隆引用的内存块。如果是非引用内存块，直接返回成功。
 * 
 * @return 0 成功，-1 失败。
 * 
 * @see good_allocator_privatize()
*/
int good_buffer_privatize(good_buffer_t *dst);

/**
 * 调整容量。
 * 
 * @return 0 成功，-1 失败。
 * 
 * @see  memcpy()
*/
int good_buffer_resize(good_buffer_t *buf,size_t size);

/**
 * 写入数据。
 * 
 * @return > 0 写入的长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see memcpy();
*/
ssize_t good_buffer_write(good_buffer_t *buf, const void *data, size_t size);

/**
 * 读取数据。
 * 
 * @return 读取的长度(Bytes)，= 0 末尾，< 0 出错。
 * 
 * @see memcpy();
*/
ssize_t good_buffer_read(good_buffer_t *buf, void *data, size_t size);

/**
 * 排出已读数据，未读数据移动到缓存首地址。
 * 
 * @see memmove();
*/
void good_buffer_drain(good_buffer_t *buf);

/**
 * 填满缓存。
 * 
 * @param stuffing 填充物。
 * @return > 0 添加的长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see memset()
*/
ssize_t good_buffer_fill(good_buffer_t *buf,uint8_t stuffing);

/**
 * 格式化写入数据。
 * 
 * @return > 0 写入的长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_vprintf(good_buffer_t *buf,const char * fmt, va_list args);

/**
 * 格式化写入数据。
 * 
 * @return > 0 写入的长度(Bytes)，= 0 已满，< 0 出错。
 * 
 * @see vsnprintf()
*/
ssize_t good_buffer_printf(good_buffer_t *buf,const char * fmt,...);

/**
 * 从文件导入数据。
 * 
 * @see good_buffer_import_atmost()
*/
ssize_t good_buffer_import(good_buffer_t *buf,int fd);

/**
 * 从文件导入数据。
 * 
 * 阻塞模式的句柄，可能会因为导入数据不足而阻塞。
 * 
 * @see good_buffer_write()
 * @see good_read()
*/
ssize_t good_buffer_import_atmost(good_buffer_t *buf,int fd,size_t howmuch);

/**
 * 导出数据到文件。
 * 
 * @see good_buffer_export_atmost()
*/
ssize_t good_buffer_export(good_buffer_t *buf,int fd);

/**
 * 导出数据到文件。
 * 
 * @see good_buffer_read()
 * @see good_write()
*/
ssize_t good_buffer_export_atmost(good_buffer_t *buf,int fd,size_t howmuch);


#endif //GOOD_UTIL_BUFFER_H