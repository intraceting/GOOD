/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "option.h"

static good_tree_t *_good_option_find_key(good_tree_t *opt, const char *key,int create)
{
    good_tree_t *it = NULL;
    int chk;

    assert(opt != NULL && key != NULL);

    it = good_tree_child(opt,1);
    while(it)
    {
        chk = good_strcmp(it->alloc->pptrs[GOOD_OPTION_KEY], key, 1);
        if (chk == 0)
            break;

        it = good_tree_sibling(it,0);
    }

    if(it == NULL && create !=0 )
    {
        it = good_tree_alloc3(strlen(key)+1);

        if(it)
        {
            strcpy(it->alloc->pptrs[GOOD_OPTION_KEY],key);
            good_tree_insert2(opt,it,0);
        }
    }

    return it;
}

static good_tree_t *_good_option_find_value(good_tree_t *key,size_t index)
{
    good_tree_t *it = NULL;
    size_t chk;

    assert(key != NULL);

    it = good_tree_child(key,1);
    while(it)
    {
        if (chk++ == index)
            break;

        it = good_tree_sibling(it,0);
    }

    return it;
}

static size_t _good_option_count_value(good_tree_t *key)
{
    good_tree_t *it = NULL;
    size_t chk = 0;

    assert(key != NULL);

    it = good_tree_child(key,1);
    while(it)
    {
        chk +=1;

        it = good_tree_sibling(it,0);
    }

    return chk;
}

int good_option_set(good_tree_t *opt, const char *key, const char *value)
{
    good_tree_t *it_key = NULL;
    good_tree_t *it_val = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _good_option_find_key(opt,key,1);
    if(!it_key)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,-1);

    /*
     * 允许没有值。
    */
    if (value == NULL || value[0] == '\0')
        return 0;
    
    it_val = good_tree_alloc3(strlen(value) + 1);
    if (!it_val)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, -1);

    strcpy(it_val->alloc->pptrs[GOOD_OPTION_VALUE], value);
    good_tree_insert2(it_key, it_val, 0);
    

    return 0;
}

const char* good_option_get(good_tree_t *opt, const char *key,size_t index,const char* defval)
{
    good_tree_t *it_key = NULL;
    good_tree_t *it_val = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _good_option_find_key(opt,key,0);
    if(!it_key)
        GOOD_ERRNO_AND_RETURN1(EAGAIN,defval);

    it_val = _good_option_find_value(it_key,index);
    if(!it_val)
        GOOD_ERRNO_AND_RETURN1(EAGAIN,defval);

    return it_val->alloc->pptrs[GOOD_OPTION_VALUE];
}

ssize_t good_option_count(good_tree_t *opt, const char *key)
{
    good_tree_t *it_key = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _good_option_find_key(opt,key,0);
    if(!it_key)
        GOOD_ERRNO_AND_RETURN1(EAGAIN,-1);

    return _good_option_count_value(it_key);
}
