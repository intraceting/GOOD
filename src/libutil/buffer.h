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

#include "macro.h"

/**
 * 获取缓存大小
 * 
*/
size_t good_buffer_size(void *buf);

/**
 * 申请缓存
 * 
 * @param size 大小
 * @param clean_cb 清理函数
 * @param opaque 环境指针
 * 
 * @return 失败返回 NULL(0)，成功返回缓存指针。
*/
void *good_buffer_alloc(size_t size, void (*clean_cb)(void *buf, void *opaque), void *opaque);

/**
 * 申请缓存
 * 
 * @see good_buffer_alloc()
*/
void *good_buffer_alloc2(size_t size);

/**
 * 缓存引用
 * 
 * @return 失败返回 NULL(0)，成功返回缓存指针。
 * 
*/
void *good_buffer_refer(void *buf);

/**
 * 缓存释放
 * 
 * @note 当缓存引用计数为0时才会释放缓存。
 * 
 * @note 如果清理函数被定义，则在释放前调用清理函数。
*/
void good_buffer_unref(void **buf);

#endif //GOOD_UTIL_BUFFER_H