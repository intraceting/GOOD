/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_BUFFER_H
#define GOOD_UTIL_BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>

#include "general.h"
#include "heap.h"

/**
 * 简单的缓存结构。
 *  
 * 这个结构需要下面的接口配合使用，直接创建使用不具有引用计数器。
*/
typedef struct _good_buffer
{
    /**
     * 数量
    */
    size_t number;

    /**
     * 缓存
     * 
     * 通过接口创建的缓存区，如果调用者覆盖了此项，自动填充的缓存区在释放时不会产生内存泄漏。
    */
    uint8_t** data;

    /**
     * 大小
     * 
     * 通过接口创建时，自动填充。
    */
    size_t* size;

    /**
     * 大小1
     * 
     * 通过接口创建时，初始化为0。
    */
    size_t* size1;

    /**
     * 大小2
     * 
     * 通过接口创建时，初始化为0。
    */
    size_t* size2;

    /**
     * 释放。
     * 
     * @note NULL(0) 忽略。
    */
    void (*free_cb)(size_t number,uint8_t **data,size_t* size,void *opaque);

    /**
     * 私有指针。
     * 
     * @see free_cb()
    */
    void *opaque;

} good_buffer_t;

/**
 * 创建缓存。
 * 
 * 内部会创建附加结构，用于引用计数器。
 * 
 * @param size 缓存大小(数组)。NULL(0) 缓存空间为0。
 * @param number 缓存数量。0 不创建缓存。
 * @param free_cb 释放函数。
 * @param opaque 环境指针。
 * 
 * @return  NULL(0) 失败，!NULL(0) 成功。
 * 
 * @see good_heap_alloc()
*/
good_buffer_t *good_buffer_alloc(size_t size[], size_t number,
                                 void (*free_cb)(size_t number, uint8_t **data, size_t *size, void *opaque), 
                                 void *opaque);

/**
 * 创建缓存。
 * 
 * @see good_buffer_alloc()
*/
good_buffer_t *good_buffer_alloc2(size_t size[],size_t number);

/**
 * 创建缓存。
 * 
 * @param size 缓存大小(缓存数量为1。)。
 * 
 * @see good_buffer_alloc()
 * @see good_buffer_alloc2()
*/
good_buffer_t *good_buffer_alloc3(size_t size);

/**
 * 引用。
 * 
 * 计数器加1。
 * 
 * @return NULL(0) 失败，成功返回缓存指针。
 * 
*/
good_buffer_t *good_buffer_refer(good_buffer_t *buf);

/**
 * 释放。
 * 
 * 计数器减1。当引用计数为0时才会真的释放内存，因此引用计数大于0时，不会影向其它使用者。
 * 
 * @param buf 缓存指针的指针。当接口返回时，被赋值NULL(0)。
 * 
 * @see free_cb()
 * @see good_heap_freep()
*/
void good_buffer_unref(good_buffer_t **buf);

#endif //GOOD_UTIL_BUFFER_H