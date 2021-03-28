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
#include <string.h>

#include "general.h"
#include "buffer.h"
#include "bloom.h"


/**
 * 树节点。
 * 
 * 比较原始的树节点结构。
*/
typedef struct _good_tree
{
    /**
     * 节点之间的关系链。
     * 
     * @note 尽量不要直接访问或修改。
    */
    struct _good_tree *chain[5];

    /**
    * 父。
    */
#define GOOD_TREE_CHAIN_FATHER          0

    /**
    * 兄长。
    */
#define GOOD_TREE_CHAIN_SIBLING_PREV    1

    /**
    * 小弟。
    */
#define GOOD_TREE_CHAIN_SIBLING_NEXT    2

    /**
    * 大娃。
    */
#define GOOD_TREE_CHAIN_CHILD_FIRST     3

    /**
    * 么娃。
    */
#define GOOD_TREE_CHAIN_CHILD_LEAST     4

    /**
     * 缓存，存放节点数据。
     * 
     * @see good_buffer_alloc()
     * @see good_buffer_alloc2()
     * @see good_buffer_alloc3()
     * @see good_buffer_refer()
     * @see good_buffer_unref()
     */
    good_buffer_t *buf;

    union
    {
        good_buffer_t stream;

        struct
        {

            good_buffer_t first;

            good_buffer_t second;

        } pair;
    } data;

}good_tree_t;

/**
 * 迭代器。
*/
typedef struct _good_tree_iterator
{
    /**
     * Must be 0.
    */
    int flags;

    /**
     * 栈。
     * 
    */
    good_buffer_t *stack;

    /**
     * 回显
     * 
     * @return -1 终止，0 忽略孩子，1 继续。
    */
    int (*dump_cb)(size_t deep,const good_tree_t *node, void *opaque);

    /**
     * 环境指针
    */
    void *opaque;

} good_tree_iterator;

/**
 * 获取自己的父节指针。
*/
good_tree_t *good_tree_father(const good_tree_t *self);

/**
 * 获取自己的兄弟指针。
 * 
 * @param elder 0 找小弟，!0 找兄长。
*/
good_tree_t *good_tree_sibling(const good_tree_t *self,int elder);

/**
 * 获取自己的孩子指针。
 * 
 * @param first 0 找么娃，!0 找大娃。
*/
good_tree_t *good_tree_child(const good_tree_t *self,int first);

/**
 * 断开自己在树中的关系链。
 * 
 * 自己的孩子，以孩子的孩子都会跟随自己从树节点中断开。
 * 
*/
void good_tree_unlink(good_tree_t *self);

/**
 * 插入节点到树的关系链中。
 * 
 * @param father 父。
 * @param child 孩子。
 * @param where NULL(0) 'child'为小弟，!NULL(0) 'child'为兄长。
 * 
*/
void good_tree_insert(good_tree_t *father, good_tree_t *child, good_tree_t *where);

/**
 * 插入节点到树的关系链中。
 * 
 * @param father 父。
 * @param child 孩子。
 * @param first 0 'child'为么娃，!0 'child'为大娃。
 * 
*/
void good_tree_insert2(good_tree_t *father, good_tree_t *child,int first); 

/**
 * 删除节点。
 * 
 * 包括自己，自己的孩子，以孩子的孩子都会被删除。
 * 
 * @param root 节点指针的指针。当接口返回时，被赋值NULL(0)。

 * @see good_heap_free()
 * @see good_buffer_unref()
*/
void good_tree_free(good_tree_t **root);

/**
 * 创建节点。
 * 
 * 可以附带创建缓存区用于数据存储。
 * 
 * @param size 缓存大小(数组)。NULL(0) 缓存空间为0。
 * @param number 缓存数量。0 不创建缓存。
 * 
 * @see good_heap_calloc()
 * @see good_buffer_alloc2()
 * 
*/
good_tree_t *good_tree_alloc(size_t size[],size_t number);

/**
 * 创建节点。
 * 
 * 可以附带创建缓存区用于数据存储。
 * 
 * @param size 缓存大小(缓存数量为1。)。NULL(0) 缓存空间为0。
 * 
 * @see good_tree_alloc()
 * 
*/
good_tree_t *good_tree_alloc2(size_t size);

/**
 * 扫描树节点。
 * 
 * 深度优先遍历节点，也可以通过迭代器控制遍历方向。
 * 
 * 如果没有准备迭代堆栈，内部自动创建2048高度的堆栈。
 * 
 * @param it 迭代器。
 * 
*/
void good_tree_scan(const good_tree_t *root,good_tree_iterator* it);

/**
 * 格式化输出。
 * 
 * 输出有层次感的树形图。
 * 
 * @note 不会在末尾添加'\n'(换行)字符。
 * 
 * @see fprintf()
*/
void good_tree_fprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,...);

/**
 * 格式化输出。
 * 
 * 输出有层次感的树形图。
 * 
 * @note 不会在末尾添加'\n'(换行)字符。
 * 
 * @see vfprintf()
*/
void good_tree_vfprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,va_list args);


#endif //GOOD_UTIL_TREE_H