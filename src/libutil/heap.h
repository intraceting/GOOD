/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_HEAP_H
#define GOOD_UTIL_HEAP_H

#include "general.h"

/**
 * 内存申请。
 * 
 * @see calloc()
 * 
 */
void* good_heap_alloc(size_t size);

/**
 * 内存重新申请。
 * 
 * @see realloc()
 * 
 */
void* good_heap_realloc(void *buf,size_t size);

/**
 * 内存释放。
 * 
 * @param data 
 * 
 * @see free()
 */
void good_heap_free(void *data);

/**
 * 内存释放。
 * 
 * @param data 指针的指针。返回时赋值NULL(0)。
 * 
 * @see good_heap_free()
 */
void good_heap_freep(void **data);

/**
 * 内存克隆。
 * 
 * @see good_heap_alloc()
 * @see memcpy()
*/
void* good_heap_clone(const void* data,size_t size);

#endif //GOOD_UTIL_HEAP_H