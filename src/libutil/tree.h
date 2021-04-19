/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_TREE_H
#define GOOD_UTIL_TREE_H

#include "general.h"
#include "allocator.h"


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
     * 数据。
     * 
     * 当节点被删除时，如果不为NULL(0)，自动调用good_allocator_unref()释放。
    */
    good_allocator_t *alloc;

}good_tree_t;

/**
 * 树节点关系。
*/
enum _good_tree_chain
{

    /**
    * 父。
    */
   GOOD_TREE_CHAIN_FATHER = 0,
#define GOOD_TREE_CHAIN_FATHER          GOOD_TREE_CHAIN_FATHER

    /**
    * 兄长。
    */
   GOOD_TREE_CHAIN_SIBLING_PREV = 1,
#define GOOD_TREE_CHAIN_SIBLING_PREV    GOOD_TREE_CHAIN_SIBLING_PREV

    /**
    * 小弟。
    */
   GOOD_TREE_CHAIN_SIBLING_NEXT = 2,
#define GOOD_TREE_CHAIN_SIBLING_NEXT    GOOD_TREE_CHAIN_SIBLING_NEXT

    /**
    * 大娃。
    */
   GOOD_TREE_CHAIN_CHILD_FIRST = 3,
#define GOOD_TREE_CHAIN_CHILD_FIRST     GOOD_TREE_CHAIN_CHILD_FIRST

    /**
    * 么娃。
    */
   GOOD_TREE_CHAIN_CHILD_LEAST = 4
#define GOOD_TREE_CHAIN_CHILD_LEAST     GOOD_TREE_CHAIN_CHILD_LEAST
};

/**
 * 树节点迭代器。
 * 
*/
typedef struct _good_tree_iterator
{
    /** 
     * 最大深度。如果无法确定填多少合适，就填0。
    */
    size_t depth_max;

    /**
     * 回显函数。
     * 
     * @return -1 终止，0 忽略孩子，1 继续。
    */
    int (*dump_cb)(size_t depth, good_tree_t *node, void *opaque);

    /**
     * 环境指针。
    */
    void *opaque;

} good_tree_iterator_t;

/**
 * 树节点排序规则。
 * 
*/
typedef struct _good_tree_order
{
    /*
     * 顺序。!0 升序，0 降序。
    */
    int by;

    /**
     * 比较函数。
     * 
     * @return > 0 is node1 > node2，0 is node1 == node2，< 0 is node1 < node2。
    */
    int (*compare_cb)(const good_tree_t *node1, const good_tree_t *node2, void *opaque);

    /**
     * 环境指针。
    */
    void *opaque;

} good_tree_order_t;

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
 * @param where NULL(0) 孩子为小弟，!NULL(0) 孩子为兄长。
 * 
*/
void good_tree_insert(good_tree_t *father, good_tree_t *child, good_tree_t *where);

/**
 * 插入节点到树的关系链中。
 * 
 * @param father 父。
 * @param child 孩子。
 * @param first 0 孩子为么娃，!0 孩子为大娃。
 * 
*/
void good_tree_insert2(good_tree_t *father, good_tree_t *child,int first); 

/**
 * 节点交换。
 * 
 * @warning 必须是同一个父节点的子节点。
 * 
*/
void good_tree_swap(good_tree_t *src,good_tree_t *dst);

/**
 * 删除节点。
 * 
 * 包括自己，自己的孩子，以孩子的孩子都会被删除。
 * 
 * @param root 节点指针的指针。当接口返回时，被赋值NULL(0)。

 * @see good_heap_free()
 * @see good_allocator_unref()
*/
void good_tree_free(good_tree_t **root);

/**
 * 创建节点。
 * 
 * @param alloc 内存块指针，可以为NULL(0)。仅复制指针，不是指针对像引用。
 * 
 * @see good_heap_alloc()
 * 
*/
good_tree_t *good_tree_alloc(good_allocator_t *alloc);

/**
 * 创建节点，同时申请数据内存块。
 * 
 * @see good_tree_alloc()
 * @see good_allocator_alloc()
*/
good_tree_t *good_tree_alloc2(size_t *sizes,size_t numbers);

/**
 * 创建节点，同时申请数据内存块。
 * 
 * @see good_tree_alloc2()
*/
good_tree_t *good_tree_alloc3(size_t size);

/**
 * 扫描树节点。
 * 
 * 深度优先遍历节点。
 * 
*/
void good_tree_scan(good_tree_t *root,good_tree_iterator_t* it);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 输出的长度，< 0 失败。
 * 
 * @see good_tree_vfprintf()
 * @see fprintf()
*/
ssize_t good_tree_fprintf(FILE* fp,size_t depth,const good_tree_t *node,const char* fmt,...);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 输出的长度，< 0 失败。
 * 
 * @see vfprintf()
*/
ssize_t good_tree_vfprintf(FILE* fp,size_t depth,const good_tree_t *node,const char* fmt,va_list args);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 输出的长度，< 0 失败。
 * 
 * @see good_tree_vfprintf()
 * @see snprintf()
*/
ssize_t good_tree_snprintf(char *buf, size_t max, size_t depth, const good_tree_t *node, const char *fmt, ...);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 输出的长度，< 0 失败。
 * 
 * @see good_tree_vfprintf()
 * @see vsnprintf()
*/
ssize_t good_tree_vsnprintf(char *buf, size_t max, size_t depth, const good_tree_t *node,const char* fmt,va_list args);

/** 
 * 排序。
 * 
 * 选择法排序。
 * 
 * 只排序子节点，如需要对整颗树排序，需要接合迭代器。
 * 
 * @see good_tree_child()
 * @see good_tree_sibling()
 * @see good_tree_swap()
*/
void good_tree_sort(good_tree_t *father,good_tree_order_t *order);


#endif //GOOD_UTIL_TREE_H