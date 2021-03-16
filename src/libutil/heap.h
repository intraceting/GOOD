/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_HEAP_H
#define GOOD_UTIL_HEAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/**
 * 内存申请
 * 
 * @see calloc()
 * 
 */
void* good_heap_alloc(size_t size);

/**
 * 内存申请
 * 
 * @see realloc()
 * 
 */
void* good_heap_realloc(void *buf,size_t size);

/**
 * 内存释放
 * 
 * @param data 
 * 
 * @see free()
 */
void good_heap_free(void *data);

/**
 * 内存释放，并清空指针。
 * 
 * @param data 指针的指针
 * 
 * @see good_heap_free()
 */
void good_heap_freep(void **data);

#endif //GOOD_UTIL_HEAP_H