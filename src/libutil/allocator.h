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
 * 获取内存块大小。
*/
size_t good_size(void *mem);

/**
 * 注册内存块释放前销毁回调方法。
 * 
 * @param destroy_cb 析构回调。NULL（0）忽略。
 * @param opaque 环境指针。
*/
void good_atfree(void *mem,
                 void (*destroy_cb)(void *m, size_t size, void *opaque),
                 void *opaque);
                 
/**
 * 申请指定大小的内存块。
 * 
 * 并且引用计数器初始化为1。
 * 
 * @param size 内存大小。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see calloc()
 */
void *good_alloc(size_t size);

/**
 * 内存块引用。
 * 
 * 只增加引用计数，不对数据做任何修改。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
*/
void *good_refer(void *mem);

/**
 * 内存块释放。
 * 
 * 当引用计数大于1时，只减少引用计数，不对数据做任何修改。
 * 当引用计数减少至1时，首先执行析构回调方法，然后才释放内存块。
 * 
 * @param mem 内存块指针的指针。函数返回前修改为NULL(0);
 * 
 * @see free()
*/
void good_unref(void **mem);

/**
 * 内存块克隆。
 * 
 * 申请新的内存块，仅复制数据和大小。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 * 
 * @see good_alloc()
 * @see memcpy()
*/
void* good_clone(const void* data,size_t size);



#endif //GOOD_UTIL_ALLOCATOR_H