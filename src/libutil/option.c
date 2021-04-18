/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "option.h"

static void _good_option_construct_cb(good_allocator_t *alloc, void *opaque)
{
    good_vector_t **val = (good_vector_t **)alloc->pptrs[GOOD_MAP_VALUE];
    good_option_t *opt = (good_option_t *)opaque;

    *val = good_vector_alloc2(sizeof(char*),0);
}

static void _good_option_destructor_cb(good_allocator_t *alloc, void *opaque)
{
    good_vector_t **val = (good_vector_t **)alloc->pptrs[GOOD_MAP_VALUE];
    char **text;

    if(*val == NULL)
        return ;

    for (size_t i = 0; i < (*val)->count; i++)
    {
        text = (char**)good_vector_at(*val, i);
        if (text == NULL)
            continue;

        good_heap_freep((void**)text);
    }

    good_vector_freep(val);
}   

int good_option_compare(const void *data1, const void *data2,void *opaque)
{
    return good_strcmp(data1,data2,1);
}

void good_option_destroy(good_option_t* opt)
{
    assert(opt != NULL);

    good_tree_free(&opt->table);

    /*
     * fill zero.
    */
    memset(opt,0,sizeof(*opt));
}

int good_option_init(good_option_t *opt)
{
    assert(opt);

    /*
     * 创建树节点，用于表格。
    */
    opt->table = good_tree_alloc(NULL);

    if (!opt->table)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, -1);

    /*
     * 如果未指定，则启用默认函数。
    */
    if (!opt->compare_cb)
        opt->compare_cb = good_option_compare;

    return 0;
}

static good_tree_t * _good_option_find(good_option_t *opt, const char *key)
{

}

int good_option_set(good_option_t *opt, const char *key, const char *value)
{
    good_allocator_t *alloc = NULL;
    int chk;

    assert(opt != NULL && key != NULL && value != NULL);
    assert(key[0] != '\0' && value[0] != '\0');
    

    return chk;
}

const char* good_option_get(const good_option_t *opt, const char *key,size_t index,const char* defval)
{
    
}