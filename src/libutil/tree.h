/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */

#ifndef GOOD_UTIL_TREE_H
#define GOOD_UTIL_TREE_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdarg.h>

#include "macro.h"
#include "buffer.h"


/**
 * 树
*/
typedef struct _good_tree
{
    /**
     * 链
    */
    struct _good_tree *chain[5];

    /**
    * 父
    */
#define GOOD_TREE_CHAIN_FATHER          0

    /**
    * 兄长
    */
#define GOOD_TREE_CHAIN_SIBLING_PREV    1

    /**
    * 小弟
    */
#define GOOD_TREE_CHAIN_SIBLING_NEXT    2

    /**
    * 大娃
    */
#define GOOD_TREE_CHAIN_CHILD_FIRST     3

    /**
    * 么娃
    */
#define GOOD_TREE_CHAIN_CHILD_LEAST     4

    /**
     * 
    */
    uintmax_t code;

    /**
     * 
    */
    size_t size;

    /**
     * 
    */
    void *data;

}good_tree_t;

/**
 * 迭代器
*/
typedef struct _good_tree_iterator
{
    /**
     * @note Must be 0.
    */
    int flags;

    /**
     * 栈高度
    */
    size_t stack_size;

    /**
     * 栈
    */
    good_tree_t **stack;

    /**
     * 回显
    */
    void (*dump_cb)(size_t deep,const good_tree_t *node, void *opaque);

    /**
     * 环境指针
    */
    void *opaque;

} good_tree_iterator;

/**
 * 父节点
*/
good_tree_t *good_tree_father(const good_tree_t *self);

/**
 * 兄弟
 * 
 * @param elder 0 返回小弟，!0 返回兄长。
*/
good_tree_t *good_tree_sibling(const good_tree_t *self,int elder);

/**
 * 孩子
 * 
 * @param first 0 返回么娃，!0 返回大娃。
*/
good_tree_t *good_tree_child(const good_tree_t *self,int first);

/**
 * 断开
 * 
*/
void good_tree_unlink(good_tree_t *self);

/**
 * 插入
 * 
 * @param father 父
 * @param child 孩子
 * @param where NULL(0) 'child'为小弟，!NULL(0) 'child'为兄长。
 * 
*/
void good_tree_insert(good_tree_t *father, good_tree_t *child, good_tree_t *where);

/**
 * 插入
 * 
 * @param father 父
 * @param child 孩子
 * @param first 0 么娃，!0 大娃。
 * 
*/
void good_tree_insert2(good_tree_t *father, good_tree_t *child,int first); 

/**
 * 清理
 * 
 * @param free_cb NULL(0) 调用good_tree_free()直接删除节点，!NULL(0) 调用此函数执行删除操作。
 * @param opaque 环境指针
 * 
 * @note 所有子节点，但不包括自身。
 * 
 * @see good_tree_free()
*/
void good_tree_clear(good_tree_t *root,void (*free_cb)(good_tree_t *node, void *opaque), void *opaque);

/**
 * 清理
 * 
 * @see good_tree_clear()
 */
void good_tree_clear2(good_tree_t *root);

/**
 * 删除
 * 
 * @note 不能有子节点。
 * 
 * @see good_tree_alloc()
 * @see good_buffer_unref()
 * 
*/
void good_tree_free(good_tree_t **root);

/**
 * 申请
 * 
 * @see good_buffer_alloc2()
*/
good_tree_t *good_tree_alloc(size_t size);

/**
 * 遍历
 * 
 * @param it 迭代器
 * 
*/
void good_tree_traversal(const good_tree_t *root,good_tree_iterator* it);

/**
 * 格式化输出
 * 
 * @note 不会在末尾添加'\n'(换行)字符。
 * 
 * @see fprintf()
*/
void good_tree_fprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,...);

/**
 * 格式化输出
 * 
 * @note 不会在末尾添加'\n'(换行)字符。
 * 
 * @see vfprintf()
*/
void good_tree_vfprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,va_list args);


#endif //GOOD_UTIL_TREE_H