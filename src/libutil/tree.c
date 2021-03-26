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

                good_buffer_unref(&node->buf);
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

    good_buffer_unref(&(*root)->buf);
    good_heap_freep((void**)root);

}

good_tree_t *good_tree_alloc(size_t size[], size_t number)
{
    good_tree_t *node = (good_tree_t *)good_heap_alloc(sizeof(good_tree_t));

    if (!node)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

    if (number > 0)
        node->buf = good_buffer_alloc2(size, number);
    else
        node->buf = NULL;

    return node;
}

good_tree_t *good_tree_alloc2(size_t size)
{
    return good_tree_alloc(&size,1);
}

void good_tree_scan(const good_tree_t *root, good_tree_iterator *it)
{
    good_tree_t *node = NULL;
    good_tree_t *child = NULL;
    size_t deep = 0;// begin 0
    int inside_stack = 0;
    int chk;

    assert(root && it && it->dump_cb);
        
    /*
     * 如果没有准备，则在内部准备。
    */
    if (!it->stack)
    {   
        inside_stack = 1;
        it->stack = good_buffer_alloc2(NULL,2048);
    }

    if (!it->stack)
        return;

    /*
     * 根
    */
    chk = it->dump_cb(0,root,it->opaque);
    if(chk == 0)
        return;

    /*
     * 从第一个孩子开始遍历。
    */
    node = good_tree_child(root,1);

    while(node)
    {
        chk = it->dump_cb(deep + 1, node, it->opaque);
        if (chk < 0)
            return;

        if(chk > 0)
            child = good_tree_child(node,1);
        else 
            child = NULL;

        if(child)
        {
            assert(it->stack->number > deep);

            it->stack->data[deep++] = (uint8_t*)node;

            node = child;
        }
        else
        {
            node = good_tree_sibling(node,0);

            while (!node && deep > 0)
            {
                node = (good_tree_t *)it->stack->data[--deep];
                node = good_tree_sibling(node,0);
            }
        }
    }

    /*
     * 不能释放外部准备的堆栈。
    */
    if(inside_stack)
        good_buffer_unref(&it->stack);

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
    good_buffer_t *stack = NULL;

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
        stack = good_buffer_alloc2(NULL,deep);
        if(!stack)
            GOOD_ERRNO_AND_RETURN0(ENOMEM);

        tmp = (good_tree_t *)node;

        for (size_t i = 1; i < deep; i++)
            stack->data[deep-i] = (uint8_t*)(tmp = good_tree_father(tmp));

        for (size_t i = 1; i < deep; i++)
        {
            if (good_tree_sibling((good_tree_t *)stack->data[i], 0))
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

    good_buffer_unref(&stack);
}