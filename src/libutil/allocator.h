/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_ALLOCATOR_H
#define GOOD_UTIL_ALLOCATOR_H

#include "general.h"

/**
 * 带引用计数器的内存块信息。
 *
 * 
*/
typedef struct _good_allocator
{
    /**
     * 引用计数器指针。
    */
    const atomic_uint *refcount;

    /**
     * 内存块数量。
    */
    size_t numbers;

    /**
     * 存放内存块大小的指针数组。
     *
     * @warning 如果此项值被调用者覆盖，则需要调用者主动释放，或注册析构函数处理。
     */
    size_t *sizes;

    /**
     * 存放内存块指针的指针数组。
     * 
     * @warning 如果此项值被调用者覆盖，则需要调用者主动释放，或注册析构函数处理。
     */
    uint8_t **pptrs;

} good_allocator_t;

/**
 * 注册内存块析构函数。
 *
 * @param opaque 环境指针。
*/
void good_allocator_atfree(good_allocator_t *alloc,
                           void (*destroy_cb)(good_allocator_t *alloc, void *opaque),
                           void *opaque);

/**
 * 申请多个内存块。
 * 
 * @param sizes 指定每个内存块的容量。NULL(0) 容量为0。
 * @param numbers 数量。> 0 的整数。
 * 
 * @see good_heap_alloc()
*/
good_allocator_t *good_allocator_alloc(size_t *sizes,size_t numbers);

/**
 * 申请一个内存块。
 * 
 * @param size 容量。>= 0 的整数。
 * 
 * @see good_allocator_alloc()
*/
good_allocator_t *good_allocator_alloc2(size_t size);

/**
 * 内存块引用。
 * 
 * 复制内存块指针。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
*/
good_allocator_t *good_allocator_refer(good_allocator_t *src);

/**
 * 内存块释放。
 * 
 * 当前是最后一个引用者才会释放。
 * 
 * @param buf 指针的指针。函数返回前设置为NULL(0)。
 * 
 * @see good_heap_free()
*/
void good_allocator_unref(good_allocator_t **dst);

/**
 * 内存块克隆。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_allocator_alloc()
 * @see memcpy()
*/
good_allocator_t *good_allocator_clone(good_allocator_t *src);

/**
 * 内存块私有化。
 * 
 * 当前是唯一引用者时，直接返回。
 * 当前不是唯一引用者时，执行克隆，然后执行释放。
 *
 * @param src 内存块指针的指针。私有化成功后，指针不可再被访问；如果私有化失败，指针依然有效。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_allocator_clone()
 * @see good_allocator_unref()
*/
good_allocator_t * good_allocator_privatize(good_allocator_t **dst);


#endif //GOOD_UTIL_ALLOCATOR_H