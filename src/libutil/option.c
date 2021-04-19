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

int good_option_remove(good_tree_t *opt, const char *key)
{
    good_tree_t *it_key = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _good_option_find_key(opt,key,0);
    if(!it_key)
        GOOD_ERRNO_AND_RETURN1(EAGAIN,-1);

    good_tree_unlink(it_key);
    good_tree_free(&it_key);

    return 0;
}

ssize_t good_option_fprintf(FILE *fp,good_tree_t *opt)
{
    good_tree_t *it_key = NULL;
    good_tree_t *it_val = NULL;
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(fp != NULL && opt != NULL);

    it_key = good_tree_child(opt,1);
    while(it_key)
    {
        wsize2 = fprintf(fp, "\n%s\n", it_key->alloc->pptrs[GOOD_OPTION_KEY]);
        if (wsize2 <= 0)
            break;

        wsize += wsize2;

        it_val = good_tree_child(it_key,1);
        while(it_val)
        {
            wsize2 = fprintf(fp,"%s\n",it_val->alloc->pptrs[GOOD_OPTION_VALUE]);
            if (wsize2 <= 0)
                break;

            wsize += wsize2;

            it_val = good_tree_sibling(it_val,0);  
        }

        it_key = good_tree_sibling(it_key,0);
    }

    return wsize;
}

ssize_t good_option_snprintf(char* buf,size_t max,good_tree_t *opt)
{
    FILE* fp = NULL;
    ssize_t wsize = 0;

    assert(buf != NULL && max >0 && opt);

    fp = fmemopen(buf,max,"w");
    if(!fp)
        return -1;

    wsize = good_option_fprintf(fp,opt);

    fclose(fp);
    
    return wsize;
}

void good_option_parse(good_tree_t *opt,int argc, char* argv[],const char *prefix)
{
    int prefix_len = 0;
    const char *it_key = NULL;

    assert(opt != NULL && argc > 0 && argv != NULL && prefix != NULL);

    assert(argv[0] != NULL && argv[0][0] != '\0' && prefix[0] != '\0');

    prefix_len = strlen(prefix);
    it_key = prefix;

    for (int i = 0; i < argc;)
    {
        if (good_strncmp(argv[i], prefix, prefix_len, 1) != 0)
        {
            good_option_set(opt,it_key, argv[i++]);
        }
        else
        {
            good_option_set(opt,it_key = argv[i++],NULL);
        }
    }
}