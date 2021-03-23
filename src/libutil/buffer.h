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
 * 缓存
*/
typedef struct _good_buffer
{
    /**
     * 数量
    */
    size_t number;

    /**
     * 缓存
    */
    uint8_t** data;

    /**
     * 大小
    */
    size_t* size;

    /**
     * 大小
     * 
     * @note 内部不使用此项。
    */
    size_t* size1;

    /**
     * 大小
     * 
     * @note 内部不使用此项。
    */
    size_t* size2;

    /**
     * 释放
     * 
     * @note NULL(0) 忽略。
    */
    void (*free_cb)(uint8_t **data,size_t number,void *opaque);

    /**
     * 私有指针
     * 
     * @see free_cb()
    */
    void *opaque;

} good_buffer_t;

/**
 * 申请
 * 
 * @param size 缓存大小(数组)
 * @param number 缓存数量
 * @param free_cb 释放函数
 * @param opaque 环境指针
 * 
 * @return  NULL(0) 失败，!NULL(0) 成功。
 * 
 * @see calloc()
*/
good_buffer_t *good_buffer_alloc(size_t size[],size_t number, void (*free_cb)(uint8_t **data,size_t number,void *opaque), void *opaque);

/**
 * 申请
 * 
 * @see good_buffer_alloc()
*/
good_buffer_t *good_buffer_alloc2(size_t size[],size_t number);

/**
 * 申请
 * 
 * @param size 大小
 * 
 * @note 仅一个。
 * 
 * @see good_buffer_alloc()
 * @see good_buffer_alloc2()
*/
good_buffer_t *good_buffer_alloc3(size_t size);

/**
 * 引用
 * 
 * @return NULL(0) 失败，成功返回缓存指针。
 * 
*/
good_buffer_t *good_buffer_refer(good_buffer_t *buf);

/**
 * 释放
 * 
 * @note 当引用计数为0时才会真的释放。
 * 
 * @see free()
*/
void good_buffer_unref(good_buffer_t **buf);

#endif //GOOD_UTIL_BUFFER_H