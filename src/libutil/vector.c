/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "vector.h"

good_vector_t* good_vector_alloc(size_t type,good_allocator_t *alloc)
{
    good_vector_t *vec = NULL;

    assert(type > 0);

    vec = good_heap_alloc(sizeof(good_vector_t));
    if (!vec)
        return NULL;

    if (alloc)
    {
        assert(alloc->numbers > 0 && alloc->pptrs[0] != NULL && alloc->sizes[0] > type);

        /*
         * 绑定内存块。
        */
        vec->alloc = alloc;
        vec->type = type;

        vec->data = vec->alloc->pptrs[0];

        /*
         * 顶格分配元素数量。
        */
        vec->count = vec->alloc->sizes[0] / vec->type;
    }
    else
    {
        /*
        * 允许空的。
        */
        vec->alloc = vec->data = NULL;
        vec->type = type;
        vec->count = 0;
    }

    return vec;
}

good_vector_t *good_vector_alloc2(size_t type,size_t count)
{
    good_vector_t *vec = NULL;
    good_allocator_t *alloc = NULL;

    assert(type > 0);

    /*
    * 允许空的。
    */
    if (count > 0)
    {
        alloc = good_allocator_alloc2(count * type);
        if (!alloc)
            return NULL;
    }
    else
    {
        alloc = NULL;
    }

    vec = good_vector_alloc(type,alloc);
    if (!vec)
        goto final_error;

    vec->count = count;

    return vec;

final_error:

    good_allocator_unref(&alloc);

    return NULL;
}

void good_vector_freep(good_vector_t **dst)
{
    good_vector_t *vec_p = NULL;

    if (!dst || !*dst)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    vec_p = *dst;

    good_allocator_unref(&vec_p->alloc);

    good_heap_freep((void **)dst);
}

good_vector_t *good_vector_copy(good_vector_t *src)
{
    good_vector_t *vec = NULL;
    good_allocator_t *alloc = NULL;

    assert(src);
    assert(src->type > 0);

    /*
     * 可能是空的。 
    */
    if (src->count > 0)
    {
        assert(src->data != NULL);

        /*
        * 如果不支持引用，则执行克隆。
        */
        if (!src->alloc)
            return vec = good_vector_clone(src);

        alloc = good_allocator_refer(src->alloc);

        vec = good_vector_alloc(src->type, alloc);
        if (!vec)
            return NULL;

        vec->count = src->count;
    }
    else
    {
        vec = good_vector_alloc(src->type, 0);
    }


    return vec;
}

good_vector_t *good_vector_clone(good_vector_t *src)
{
    good_vector_t *vec = NULL;

    assert(src);
    assert(src->type > 0);

    /*
     * 可能是空的。 
    */
    if (src->count > 0)
    {
        assert(src->data != NULL);

        vec = good_vector_alloc2(src->type, src->count);
        if (!vec)
            goto final;

        vec->count = src->count;

        memcpy(vec->data, src->data, src->count * src->type);
    }
    else
    {
        vec = good_vector_alloc(src->type, 0);
    }

final:

    return vec;
}

int good_vector_privatize(good_vector_t *dst)
{
    good_allocator_t *new_p = NULL;

    assert(dst);

    if (dst->alloc)
    {
        new_p = good_allocator_privatize(&dst->alloc);
        if (!new_p)
            GOOD_ERRNO_AND_RETURN1(ENOMEM, -1);

        /*
         * 旧的指针换成新的指针。
        */
        dst->alloc = new_p;

        dst->data = new_p->pptrs[0];
    }

    return 0;
}

int good_vector_resize(good_vector_t *vec, size_t count)
{
    good_allocator_t *alloc_new = NULL;

    assert(vec != NULL && count > 0);

    if (vec->count == count)
        return 0;

    alloc_new = good_allocator_alloc2(count * vec->type);
    if (!alloc_new)
        return -1;

    /*
     * 复制数据。
    */
    memcpy(alloc_new->pptrs[0], vec->data, GOOD_MIN(vec->count,count) * vec->type);

    /*
     * 解除旧的内存块。
     */
    good_allocator_unref(&vec->alloc);

    /*
     * 绑定新的内存块。
    */
    vec->alloc = alloc_new;

    vec->data = alloc_new->pptrs[0];

    vec->count = count;

    return 0;
}

void *good_vector_at(good_vector_t *vec, size_t index)
{
    assert(vec);

    if (good_vector_privatize(vec) != 0)
        GOOD_ERRNO_AND_RETURN1(EMLINK, NULL);

    assert(vec->count > index);

    return GOOD_PTR2PTR(void, vec->data, index * vec->type);
}

void good_vector_set(good_vector_t *vec,size_t index,const void *data)
{
    void *p = good_vector_at(vec,index);

    assert(p != NULL);

    memcpy(p,data,vec->type);
}

void good_vector_get(good_vector_t *vec,size_t index,const void *data)
{
    void *p = good_vector_at(vec,index);

    assert(p != NULL);

    memcpy(data,p,vec->type);
}

int good_vector_push_back(good_vector_t *vec, const void *data)
{
    int chk;
    void *p;

    assert(vec);

    chk = good_vector_resize(vec, vec->count + 1);
    if (chk != 0)
        return -1;

    p = good_vector_at(vec, vec->count - 1);

    memcpy(p, data, vec->type);

    return 0;
}

void good_vector_pop_back(good_vector_t *vec)
{
    assert(vec);
    assert(vec->count > 0);

    vec->count -= 1;
}