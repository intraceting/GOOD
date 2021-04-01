/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "tree.h"


good_tree_t *good_tree_father(const good_tree_t *self)
{
    assert(self);

    return self->chain[GOOD_TREE_CHAIN_FATHER];
}

good_tree_t *good_tree_sibling(const good_tree_t *self,int elder)
{
    assert(self);

    if(elder)
        return self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
    
    return self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];
}


good_tree_t *good_tree_child(const good_tree_t *self,int first)
{
    assert(self);

    if(first)
        return self->chain[GOOD_TREE_CHAIN_CHILD_FIRST];

    return self->chain[GOOD_TREE_CHAIN_CHILD_LEAST];
}

void good_tree_unlink(good_tree_t *self)
{
    good_tree_t *root = NULL;

    assert(self);

    /*
     * 获取父节点
     */
    root = self->chain[GOOD_TREE_CHAIN_FATHER];

    if (!root)
        return;

    /*
     * 首<--->NODE<--->尾
     * 首<--->NODE
     * NODE<--->尾
     */
    if (self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT])
        self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
    if (self->chain[GOOD_TREE_CHAIN_SIBLING_PREV])
        self->chain[GOOD_TREE_CHAIN_SIBLING_PREV]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];

    /*
     * NODE 是首?
    */
    if (self == root->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
    {
        root->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT];
        if (root->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
            root->chain[GOOD_TREE_CHAIN_CHILD_FIRST]->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = NULL;
    }

    /*
     * NODE 是尾?
    */
    if (self == root->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
    {
        root->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = self->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
        if (root->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
            root->chain[GOOD_TREE_CHAIN_CHILD_LEAST]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = NULL;
    }

    /*
     * 打断与父节点的关系链，但同时保留子节点关系链。
    */
    self->chain[GOOD_TREE_CHAIN_FATHER] = NULL;
    self->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = NULL;
    self->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = NULL;
}

void good_tree_insert(good_tree_t *father, good_tree_t *child, good_tree_t *where)
{
    assert(father && child);

    /* 
     * 必须是根节点，或独立节点。
    */
    assert(NULL == child->chain[GOOD_TREE_CHAIN_FATHER]);
    assert(NULL == child->chain[GOOD_TREE_CHAIN_SIBLING_PREV]);
    assert(NULL == child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]);

    /* 
     * 绑定新父节点。
    */
    child->chain[GOOD_TREE_CHAIN_FATHER] = father;

    if (where)
    {
        assert(father = where->chain[GOOD_TREE_CHAIN_FATHER]);

        if (where == father->chain[GOOD_TREE_CHAIN_CHILD_FIRST])
        {
            /*
             * 添加到头节点之前。
            */
            where->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = where;

            /* 
             * 新的头节点。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = child;
        }
        else
        {
            /*
             * 添加到节点之前
            */
            where->chain[GOOD_TREE_CHAIN_SIBLING_PREV]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = where->chain[GOOD_TREE_CHAIN_SIBLING_PREV];
            child->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = where;
            where->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = child;
        }
    }
    else
    {
        if (father->chain[GOOD_TREE_CHAIN_CHILD_LEAST])
        {
            /*
             * 添加到尾节点之后。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST]->chain[GOOD_TREE_CHAIN_SIBLING_NEXT] = child;
            child->chain[GOOD_TREE_CHAIN_SIBLING_PREV] = father->chain[GOOD_TREE_CHAIN_CHILD_LEAST];

            /* 
             * 新的尾节点。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = child;
        }
        else
        {
            /*
             * 空链表，添加第一个节点。
            */
            father->chain[GOOD_TREE_CHAIN_CHILD_LEAST] = father->chain[GOOD_TREE_CHAIN_CHILD_FIRST] = child;
        }
    }
}

void good_tree_insert2(good_tree_t *father, good_tree_t *child,int first)
{
    good_tree_t* where = NULL;

    assert(father && child);

    if(first)
        where = good_tree_child(father,1);

    good_tree_insert(father,child,where);
}


void good_tree_free(good_tree_t **root)
{
    good_tree_t *root_p = NULL;
    good_tree_t *father = NULL;
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;

    if(!root || !*root)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    /*
     * 复制一下
    */
    root_p = *root;

    /*
     * 以防清理到父和兄弟节点。 
    */
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_FATHER]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_SIBLING_PREV]);
    assert(NULL == root_p->chain[GOOD_TREE_CHAIN_SIBLING_NEXT]);

    while (root_p)
    {
        node = good_tree_child(root_p,0);

        if (node)
        {
            child = good_tree_child(node,0);

            /*
             * 检测是否有子节点，如果有先清理子节点。
            */
            if (child)
            {
                root_p = node;
            }
            else
            {
                good_tree_unlink(node);

                good_allocator_unref(&node->alloc);
                good_heap_freep((void**)&node);
            }
        }
        else
        {
            /*
             * 没有子节点，返回到父节点。
            */
            root_p = good_tree_father(root_p);
        }
    }

    good_allocator_unref(&(*root)->alloc);
    good_heap_freep((void**)root);

}

good_tree_t *good_tree_alloc()
{
    good_tree_t *node = (good_tree_t *)good_heap_alloc(sizeof(good_tree_t));

    if (!node)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

    return node;
}

void good_tree_scan(const good_tree_t *root,size_t stack_deep,
                    int (*dump_cb)(size_t deep, const good_tree_t *node, void *opaque),
                    void *opaque)
{
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;
    good_tree_t **stack = NULL;
    size_t deep = 0;// begin 0
    int chk;

    assert(root && dump_cb);
        
    /*
     * 如果调用者不确定，则在内部自动确定。
    */
    if (stack_deep <= 0)
        stack_deep = PATH_MAX/2;

    stack = (good_tree_t **)good_heap_alloc(stack_deep * sizeof(good_tree_t *));
    if (!stack)
        goto final;

    /*
     * 根
    */
    chk = dump_cb(0,root,opaque);
    if(chk == 0)
        goto final;

    /*
     * 从第一个孩子开始遍历。
    */
    node = good_tree_child(root,1);

    while(node)
    {
        chk = dump_cb(deep + 1, node, opaque);
        if (chk < 0)
            goto final;

        if(chk > 0)
            child = good_tree_child(node,1);
        else 
            child = NULL;

        if(child)
        {
            assert(stack_deep > deep);

            stack[deep++] = node;

            node = child;
        }
        else
        {
            node = good_tree_sibling(node,0);

            while (!node && deep > 0)
            {
                node = stack[--deep];
                node = good_tree_sibling(node,0);
            }
        }
    }

final:

    good_heap_freep((void**)&stack);

}

void good_tree_fprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,...)
{
    va_list vaptr;
    va_start(vaptr, fmt);

    good_tree_vfprintf(fp,deep,node,fmt,vaptr);

    va_end(vaptr);
}

void good_tree_vfprintf(FILE* fp,size_t deep,const good_tree_t *node,const char* fmt,va_list args)
{
    good_tree_t *tmp = NULL;
    good_tree_t **stack = NULL;

    assert(fp && node && fmt);

    if (deep <= 0)
    {
        vfprintf(fp,fmt,args);
    }
    else
    {
        /*
         * 准备堆栈。
        */
        stack = good_heap_alloc(deep * sizeof(good_tree_t *));
        if(!stack)
            GOOD_ERRNO_AND_RETURN0(ENOMEM);

        tmp = (good_tree_t *)node;

        for (size_t i = 1; i < deep; i++)
            stack[deep-i] = (tmp = good_tree_father(tmp));

        for (size_t i = 1; i < deep; i++)
        {
            if (good_tree_sibling((good_tree_t *)stack[i], 0))
                fprintf(fp, "│   ");
            else
                fprintf(fp, "    ");
        }

        if(good_tree_sibling(node,0))
            fprintf(fp,"├── ");
        else 
            fprintf(fp,"└── ");

        vfprintf(fp,fmt,args);
    }

    good_heap_freep((void**)&stack);
}