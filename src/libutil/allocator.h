/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_ALLOCATOR_H
#define GOOD_UTIL_ALLOCATOR_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>

#include "general.h"


/**
 * 获取内存块大小。
*/
size_t good_size(void *mem);

/**
 * 申请指定大小的内存块。
 * 
 * 支持构造和析构回调方法，并且支持引用计数器。
 * 
 * @param size 内存大小。
 * @param construct_cb 构造回调。NULL（0）忽略。
 * @param destroy_cb 析构回调。NULL（0）忽略。
 * @param opaque 环境指针。
 * 
 * @return !NULL(0) 成功，NULL(0) 失败。
 */
void *good_alloc(size_t size,
                 void (*construct_cb)(void *m, size_t size, void *opaque),
                 void (*destroy_cb)(void *m, size_t size, void *opaque),
                 void *opaque);
/**
 * 内存块引用。
 * 
 * 只增加引用计数，不对数据做任何修改。
*/
void *good_refer(void *mem);

/**
 * 内存块释放。
 * 
 * 当引用计数大于1时，只减少引用计数，不对数据做任何修改。
 * 当引用计数减少至1时，首先执行析构回调方法，然后才释放内存块。
 * 
 * @param mem 内存块指针的指针。函数返回前修改为NULL(0);
*/
void good_unref(void **mem);


#endif //GOOD_UTIL_ALLOCATOR_H