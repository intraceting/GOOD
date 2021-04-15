/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "option.h"

void _good_option_table_erase_cb(good_allocator_t *alloc, void *opaque)
{

}

void good_option_destroy(good_option_t* opt)
{
    if(!opt)
        GOOD_ERRNO_AND_RETURN0(EINVAL);
    
    good_map_destroy(&opt->table);

    if(opt->prefix)
        good_heap_free((void*)opt->prefix);

    /*
     * fill zero.
    */
    memset(opt,0,sizeof(*opt));
}

int good_option_init(good_option_t *opt, const char *prefix)
{
    assert(opt != NULL && prefix != NULL);
    assert(prefix[0] != '\0');

    opt->prefix = good_heap_clone(prefix, strlen(prefix) + 1);
    if(!opt->prefix)
        goto final_error;

    if(good_map_init(&opt->table,100) != 0)
        goto final_error;

    /*
     * 注册清理回调函数。
    */
    opt->table.erase_cb = _good_option_table_erase_cb;
    opt->table.opaque = opt;

    return 0;

final_error:

    /*
     * 走到这里出错了。
    */
    good_option_destroy(opt);

    GOOD_ERRNO_AND_RETURN1(ENOMEM,-1);
}