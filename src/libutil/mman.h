/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_MMAN_H
#define GOOD_UTIL_MMAN_H

#include "general.h"
#include "allocator.h"

/**
 * 映射已经打开文件句柄到内存页面。
 *
 * @return NULL(0) 失败，!NULL(0) 成功。
 * 
 * @see fstat()
 * @see mmap()
 * @see munmap()
 * @see good_allocator_alloc()
 * @see good_allocator_atfree()
 *
*/
good_allocator_t* good_mmap(int fd,int rw,int shared);

/**
 * 映射文件到内存页面。
 * 
 * @return NULL(0) 失败，!NULL(0) 成功。
 *  
 * @see good_open()
 * @see good_mmap()
 * @see good_closep()
 *  
*/
good_allocator_t* good_mmap2(const char* name,int rw,int shared);

/**
 * 刷新数据。
 * 
 * 如果映射的内存页面是私有模式，对数据修改不会影响原文件。
 * 
 * @param async 0 同步，!0 异步。
 * 
 * @return 0 成功，-1 失败。
*/
int good_msync(good_allocator_t* alloc,int async);

/**
 * 解除内存页面映射。
 * 
 * 当引用计数为0时才会解除映射，因此引用计数大于0时，不会影向其它使用者。
 * 
 * @param alloc 指针的指针。当接口返回时，被赋值NULL(0)。
 * 
 * @see good_allocator_unref()
 * 
*/
void good_munmap(good_allocator_t** alloc);


#endif //GOOD_UTIL_MMAN_H