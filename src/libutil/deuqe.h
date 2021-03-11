/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */

#ifndef GOOD_UTIL_DEQUE_H
#define GOOD_UTIL_DEQUE_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "macro.h"
#include "buffer.h"
#include "tree.h"

/**
 * 双向链表
*/
typedef struct _good_tree _good_deque;
typedef struct _good_tree good_deque_t;

/**
 * 滚动
 * 
 * @param self NULL(0) 最前或最后，!NULL(0) 上一个或下一个。
 * @param front 0 向后滚动，!0 向前滚动。
*/
good_deque_t *good_deque_scroll(good_deque_t *deque, good_deque_t *self,int front);

/**
 * 拆分
*/
void good_deque_detach(good_deque_t *deque, good_deque_t *self);

/**
 * 弹出
 * 
 * @param back 0 弹出最前的，!0 弹出最后的。
*/
good_deque_t *good_deque_pop(good_deque_t *deque,int back);

/**
 * 插入
 * 
 * @param where NULL(0) 'self'推送到最后，!NULL(0) 'self'在之前。
 * 
*/
void good_deque_insert(good_deque_t *deque, good_deque_t *self,good_deque_t *where);

/**
 * 推送
 * 
 * @param front 0 推送到最后，!0 推送到最前。
 * 
*/
void good_deque_push(good_deque_t *deque,good_deque_t *self,int front);


/**
 * 清理
 * 
 * @param free_cb NULL(0) 调用good_deque_free()直接删除节点，!NULL(0) 调用此函数执行删除操作。
 * @param opaque 环境指针
 * 
 * @note 包括所有节点。
 * 
 * @see good_buffer_unref()
*/
void good_deque_clear(good_deque_t *deque,void (*free_cb)(good_deque_t *node, void *opaque), void *opaque);

/**
 * 清理
 * 
 * @see good_deque_clear()
*/
void good_deque_clear2(good_deque_t *deque);

/**
 * 删除
 * 
*/
void good_deque_free(good_deque_t **self);

/**
 * 申请
 * 
*/
good_deque_t *good_deque_alloc(size_t size);


#endif //GOOD_UTIL_DEQUE_H