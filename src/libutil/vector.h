/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_VECTOR_H
#define GOOD_UTIL_VECTOR_H

#include "general.h"
#include "allocator.h"

/**
 * 简单的向量容器。
 * 
*/
typedef struct _good_vector
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
     * 元素类型。
     * 
    */
    size_t type;

    /**
     * 元素数量。
    */
    size_t count;


} good_vector_t;

/**
 * 创建。
 * 
 * @param type 元素类型(Bytes)。
 * @param alloc 内存块的指针，可以为NULL(0)。仅复制指针，不是指针对像引用。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_heap_alloc()
 * @see good_heap_freep()
 * 
*/
good_vector_t* good_vector_alloc(size_t type,good_allocator_t *alloc);

/**
 * 创建。
 * 
 * @param type 元素类型(Bytes)。
 * @param count 元素数量。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_vector_alloc()
 * @see good_allocator_alloc2()
 * @see good_allocator_unref()
*/
good_vector_t* good_vector_alloc2(size_t type,size_t count);

/**
 * 释放。
 * 
 * @param dst 向量指针的指针。函数返回前修改为NULL(0);
 * 
 * @see good_heap_freep()
 * @see good_allocator_unref()
*/
void good_vector_freep(good_vector_t **dst);

/**
 * 复制。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_heap_alloc()
 * @see good_vector_alloc()
 * @see good_allocator_refer()
 */
good_vector_t *good_vector_copy(good_vector_t *src);

/**
 * 克隆。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_vector_alloc2()
 * @see memcpy()
 */
good_vector_t *good_vector_clone(good_vector_t *src);

/**
 * 私有化。
 * 
 * 用于写前复制，或克隆引用的内存块。如果是非引用的内存块，直接返回成功。
 * 
 * @return 0 成功，-1 失败。
 * 
 * @see good_allocator_privatize()
*/
int good_vector_privatize(good_vector_t *dst);

/**
 * 调整容量。
 * 
 * @return 0 成功，-1 失败。
 * 
 * @see  memcpy()
*/
int good_vector_resize(good_vector_t *vec,size_t count);

/**
 * 定位元素。
 * 
 * @param index 索引。Begin 0。
 * 
 * @return !NULL(0) 元素的指针，NULL(0) 失败。
*/
void* good_vector_at(good_vector_t *vec,size_t index);

/**
 * 元素赋值。
*/
void good_vector_set(good_vector_t *vec,size_t index,const void *data);

/**
 * 元素取值。
*/
void good_vector_get(good_vector_t *vec,size_t index,void *data);

/**
 * 向末尾添加元素。
 *
 * @return 0 成功，-1 失败。
 * 
 * @see good_vector_resize()
 * @see good_vector_at()
 * @see memcpy()
*/
int good_vector_push_back(good_vector_t *vec,const void *data);

/**
 * 弹出末尾的元素。
 * 
*/
void good_vector_pop_back(good_vector_t *vec);


#endif //GOOD_UTIL_VECTOR_H