/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */

#ifndef GOOD_UTIL_TREE_H
#define GOOD_UTIL_TREE_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

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
#define GOOD_TREE_CHAIN_FATHER  0

    /**
    * 兄长
    */
#define GOOD_TREE_CHAIN_PREV    1

    /**
    * 小弟
    */
#define GOOD_TREE_CHAIN_NEXT    2

    /**
    * 大娃
    */
#define GOOD_TREE_CHAIN_HEAD    3

    /**
    * 么娃
    */
#define GOOD_TREE_CHAIN_TAIL    4


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
 * 父
*/
good_tree_t *good_tree_father(good_tree_t *root);

/**
 * 兄长
*/
good_tree_t *good_tree_prev(good_tree_t *root);

/**
 * 小弟
*/
good_tree_t *good_tree_next(good_tree_t *root);

/**
 * 大娃
*/
good_tree_t *good_tree_head(good_tree_t *root);

/**
 * 么娃
*/
good_tree_t *good_tree_tail(good_tree_t *root);

/**
 * 节点分离
 * 
*/
void good_tree_detach(good_tree_t *node);

/**
 * 节点插入
 * 
 * @param root 根节点
 * @param node 新节点
 * @param where 插入点。 NULL(0) 新节点添加到末尾，!NULL(0) 新节点添加到'where'节点之前。
 * 
*/
void good_tree_insert(good_tree_t *root, good_tree_t *node, good_tree_t *where);

/**
 * 弹出（分离）大娃
*/
good_tree_t *good_tree_pop_head(good_tree_t *root);

/**
 * 弹出（分离）么娃
*/
good_tree_t *good_tree_pop_tail(good_tree_t *root);

/**
 * 推送（插入）兄长
*/
void good_tree_push_head(good_tree_t *root, good_tree_t *node);

/**
 * 推送（插入）小弟
*/
void good_tree_push_back(good_tree_t *root, good_tree_t *node);

/**
 * 删除节点
 * 
 * @param free_cb NULL(0) 调用good_buffer_unref()直接删除节点，!NULL(0) 调用此函数执行删除操作。
 * @param opaque free_cb() 环境指针
 * 
 * @note 包括所有子节点。
 * 
 * @see good_buffer_unref()
*/
void good_tree_free(good_tree_t **root,void (*free_cb)(good_tree_t *node, void *opaque), void *opaque);

/**
 * 申请节点
 * 
 * @see good_buffer_alloc()
*/
good_tree_t *good_tree_alloc(size_t size);



#endif //GOOD_UTIL_TREE_H