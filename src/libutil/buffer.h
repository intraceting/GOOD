/*
 * This file is part of CWheel.
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
 * @return 失败返回 NULL(0)，成功返回缓存指针。
*/
void* good_buffer_alloc(size_t size);

/**
 * 缓存引用
 * 
 * @return 失败返回 NULL(0)，成功返回缓存指针。
*/
void *good_buffer_refer(void *buf);

/**
 * 缓存释放
 * 
 * @note 当缓存引用计数为0时才会释放缓存。
*/
void good_buffer_unref(void **buf);

#endif //GOOD_UTIL_BUFFER_H