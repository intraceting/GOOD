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
 * 简单的向量。
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
     * 容量大小。
    */
    size_t size;

    /**
     * 数据类型。
    */
    size_t type;

}good_vector_t;


int good_vector_init();

int good_vector_push(const void *data,size_t size);


#endif //GOOD_UTIL_VECTOR_H