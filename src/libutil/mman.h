/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_MMAN_H
#define GOOD_UTIL_MMAN_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <sys/mman.h>

#include "general.h"
#include "heap.h"
#include "buffer.h"
#include "file.h"

/**
 * 映射已经打开文件句柄到内存页面。
 *
 * @return NULL(0) 失败，!NULL(0) 成功。
 * 
 * @see fstat()
 * @see mmap()
 * @see good_buffer_alloc()
 * @see good_buffer_refer()
 *
*/
good_buffer_t* good_mmap(int fd,int rw,int share);

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
good_buffer_t* good_mmap2(const char* name,int rw,int share);

/**
 * 刷新数据。
 * 
 * 如果映射的内存页面是私有模式，则对内存数据修改不会影响原文件。
 * 
 * @param async 0 同步，!0 异步。
 * 
 * @return 0 成功，-1 失败。
*/
int good_msync(good_buffer_t* buf,int async);

/**
 * 解除内存页面映射。
 * 
 * 当引用计数为0时才会解除映射，因此引用计数大于0时，不会影向其它使用者。
 * 
 * @param buf 缓存指针的指针。当接口返回时，被赋值NULL(0)。
 * 
 * @see good_buffer_unref()
 * @see munmap()
*/
void good_munmap(good_buffer_t** buf);

/**
 * 打开共享内存文件。
 * 
 * @return >= 0 句柄，-1 失败。
 * 
 * @note 通常是在'/dev/shm/'目录内创建。
 * 
 * @see shm_open()
*/
int good_shm_open(const char* name,int rw, int create);

/**
 * 删除共享内存文件。
 * 
 * @return 0 成功，-1 失败。
 * 
 * @see shm_unlink()
*/
int good_shm_unlink(const char* name);

#endif //GOOD_UTIL_MMAN_H