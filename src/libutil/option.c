/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "option.h"

#define GOOD_OPTION_VALUES_MAX  1000

static void _good_option_construct_cb(good_allocator_t *alloc, void *opaque)
{
    good_option_value_t *val = (good_option_value_t *)alloc->pptrs[GOOD_MAP_VALUE];
    good_option_t *opt = (good_option_t *)opaque;

}

static void _good_option_destructor_cb(good_allocator_t *alloc, void *opaque)
{
    good_option_value_t *val = (good_option_value_t *)alloc->pptrs[GOOD_MAP_VALUE];

}

void good_option_destroy(good_option_t* opt)
{
    assert(opt != NULL);

    good_map_destroy(&opt->table);

    /*
     * fill zero.
    */
    memset(opt,0,sizeof(*opt));
}

int good_option_init(good_option_t *opt)
{
    assert(opt != NULL);

    if(good_map_init(&opt->table,100) != 0)
        return -1;

    /*
     * 注册析构函数。
    */
    opt->table.construct_cb = _good_option_construct_cb;
    opt->table.destructor_cb = _good_option_destructor_cb;
    opt->table.opaque = opt;

    return 0;
}

int good_option_set(good_option_t *opt, const char *key, const char *value)
{
    good_allocator_t *alloc = NULL;
    good_option_value_t *val = NULL;
    char *value_cp = NULL;

    assert(opt != NULL && key != NULL && value != NULL);
    assert(key[0] != '\0' && value[0] != '\0');

    /*
     * 新的KEY在构造函数初始化，析构函数反初始化。
    */
    alloc = good_map_find(&opt->table, key, strlen(key), sizeof(good_option_value_t) + GOOD_OPTION_VALUES_MAX * sizeof(char *));
    if (!alloc)
        return -1;

    val = (good_option_value_t *)alloc->pptrs[GOOD_MAP_VALUE];


    return 0;
}

const good_option_value_t* good_option_get(const good_option_t *opt, const char *key)
{
    good_allocator_t *alloc = NULL;
    good_option_value_t *val = NULL;

    assert(opt != NULL && key != NULL);
    assert(key[0] != '\0');

    alloc = good_map_find((good_map_t*)&opt->table, key, strlen(key), 0);
    if (!alloc)
        GOOD_ERRNO_AND_RETURN1(EAGAIN,NULL);

    val = (good_option_value_t *)alloc->pptrs[GOOD_MAP_VALUE];

    return val;
}