/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_TREE_H
#define ABTKUTIL_TREE_H

#include "general.h"
#include "allocator.h"


/**
 * 树节点。
 * 
 * 比较原始的树节点结构。
*/
typedef struct _abtk_tree
{
    /**
     * 节点之间的关系链。
     * 
     * @note 尽量不要直接访问或修改。
    */
    struct _abtk_tree *chain[5];

    /**
     * 颜色。
    */
    int color;

    /**
     * 数据。
     * 
     * 当节点被删除时，如果不为NULL(0)，自动调用abtk_allocator_unref()释放。
    */
    abtk_allocator_t *alloc;

}abtk_tree_t;

/**
 * 树节点关系。
*/
enum _abtk_tree_chain
{

    /** 父。*/
   ABTK_TREE_CHAIN_FATHER = 0,
#define ABTK_TREE_CHAIN_FATHER          ABTK_TREE_CHAIN_FATHER

    /** 兄长。*/
   ABTK_TREE_CHAIN_SIBLING_PREV = 1,
#define ABTK_TREE_CHAIN_SIBLING_PREV    ABTK_TREE_CHAIN_SIBLING_PREV

    /** 小弟。*/
   ABTK_TREE_CHAIN_SIBLING_NEXT = 2,
#define ABTK_TREE_CHAIN_SIBLING_NEXT    ABTK_TREE_CHAIN_SIBLING_NEXT

    /** 大娃。*/
   ABTK_TREE_CHAIN_CHILD_FIRST = 3,
#define ABTK_TREE_CHAIN_CHILD_FIRST     ABTK_TREE_CHAIN_CHILD_FIRST

    /** 么娃。*/
   ABTK_TREE_CHAIN_CHILD_LEAST = 4
#define ABTK_TREE_CHAIN_CHILD_LEAST     ABTK_TREE_CHAIN_CHILD_LEAST

};

/**
 * 树节点颜色。
*/
enum _abtk_tree_color
{
    /** 无。*/
    ABTK_TREE_COLOR_NONE = 0,
#define ABTK_TREE_COLOR_NONE    ABTK_TREE_COLOR_NONE

    /** 红。*/
    ABTK_TREE_COLOR_RED = 1,
#define ABTK_TREE_COLOR_RED    ABTK_TREE_COLOR_RED

    /** 黑。*/
    ABTK_TREE_COLOR_BLACK = 2
#define ABTK_TREE_COLOR_BLACK    ABTK_TREE_COLOR_BLACK

};

/**
 * 树节点迭代器。
*/
typedef struct _abtk_tree_iterator
{
    /** 
     * 最大深度。
     * 
     * 如果无法确定填多少合适，就填0。
    */
    size_t depth_max;

    /**
     * 回显函数。
     * 
     * @return -1 终止，0 忽略孩子，1 继续。
    */
    int (*dump_cb)(size_t depth, abtk_tree_t *node, void *opaque);

    /**
     * 环境指针。
    */
    void *opaque;

} abtk_tree_iterator_t;

/**
 * 树节点排序规则。
 * 
*/
typedef struct _abtk_tree_order
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
    int (*compare_cb)(const abtk_tree_t *node1, const abtk_tree_t *node2, void *opaque);

    /**
     * 环境指针。
    */
    void *opaque;

} abtk_tree_order_t;

/**
 * 获取自己的父节指针。
*/
abtk_tree_t *abtk_tree_father(const abtk_tree_t *self);

/**
 * 获取自己的兄弟指针。
 * 
 * @param elder 0 找小弟，!0 找兄长。
*/
abtk_tree_t *abtk_tree_sibling(const abtk_tree_t *self,int elder);

/**
 * 获取自己的孩子指针。
 * 
 * @param first 0 找么娃，!0 找大娃。
*/
abtk_tree_t *abtk_tree_child(const abtk_tree_t *self,int first);

/**
 * 断开自己在树中的关系链。
 * 
 * 自己的孩子，以孩子的孩子都会跟随自己从树节点中断开。
 * 
*/
void abtk_tree_unlink(abtk_tree_t *self);

/**
 * 插入节点到树的关系链中。
 * 
 * @param father 父。
 * @param child 孩子。
 * @param where NULL(0) 孩子为小弟，!NULL(0) 孩子为兄长。
 * 
*/
void abtk_tree_insert(abtk_tree_t *father, abtk_tree_t *child, abtk_tree_t *where);

/**
 * 插入节点到树的关系链中。
 * 
 * @param father 父。
 * @param child 孩子。
 * @param first 0 孩子为么娃，!0 孩子为大娃。
 * 
*/
void abtk_tree_insert2(abtk_tree_t *father, abtk_tree_t *child,int first); 

/**
 * 节点交换。
 * 
 * @warning 必须是同一个父节点的子节点。
 * 
*/
void abtk_tree_swap(abtk_tree_t *src,abtk_tree_t *dst);

/**
 * 删除节点。
 * 
 * 包括自己，自己的孩子，以孩子的孩子都会被删除。
 * 
 * @param root 节点指针的指针。当接口返回时，被赋值NULL(0)。
*/
void abtk_tree_free(abtk_tree_t **root);

/**
 * 创建节点。
 * 
 * @param alloc 内存块指针，可以为NULL(0)。仅复制指针，不是指针对像引用。
 * 
*/
abtk_tree_t *abtk_tree_alloc(abtk_allocator_t *alloc);

/**
 * 创建节点，同时申请数据内存块。
*/
abtk_tree_t *abtk_tree_alloc2(size_t *sizes,size_t numbers,int drag);

/**
 * 创建节点，同时申请数据内存块。
*/
abtk_tree_t *abtk_tree_alloc3(size_t size);

/**
 * 扫描树节点。
 * 
 * 深度优先遍历节点。
*/
void abtk_tree_scan(abtk_tree_t *root,abtk_tree_iterator_t* it);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 成功(输出的长度)，< 0 失败。
*/
ssize_t abtk_tree_fprintf(FILE* fp,size_t depth,const abtk_tree_t *node,const char* fmt,...);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 成功(输出的长度)，< 0 失败。
*/
ssize_t abtk_tree_vfprintf(FILE* fp,size_t depth,const abtk_tree_t *node,const char* fmt,va_list args);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 输出的长度，< 0 失败。
*/
ssize_t abtk_tree_snprintf(char *buf, size_t max, size_t depth, const abtk_tree_t *node, const char *fmt, ...);

/**
 * 格式化打印。
 * 
 * 输出有层次感的树形图。
 * 
 * @warning 不会在末尾添加'\n'(换行)字符。
 * 
 * @return >=0 输出的长度，< 0 失败。
*/
ssize_t abtk_tree_vsnprintf(char *buf, size_t max, size_t depth, const abtk_tree_t *node,const char* fmt,va_list args);

/** 
 * 排序。
 * 
 * 选择法排序。
 * 
 * 只排序子节点，如需要对整颗树排序，需要接合迭代器。
*/
void abtk_tree_sort(abtk_tree_t *father,abtk_tree_order_t *order);


#endif //ABTKUTIL_TREE_H