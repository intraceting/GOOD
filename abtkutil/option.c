/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "option.h"

static abtk_tree_t *_abtk_option_find_key(abtk_tree_t *opt, const char *key,int create)
{
    abtk_tree_t *it = NULL;
    int chk;

    assert(opt != NULL && key != NULL);

    it = abtk_tree_child(opt,1);
    while(it)
    {
        chk = abtk_strcmp(it->alloc->pptrs[ABTK_OPTION_KEY], key, 1);
        if (chk == 0)
            break;

        it = abtk_tree_sibling(it,0);
    }

    if(it == NULL && create !=0 )
    {
        it = abtk_tree_alloc3(strlen(key)+1);

        if(it)
        {
            strcpy(it->alloc->pptrs[ABTK_OPTION_KEY],key);
            abtk_tree_insert2(opt,it,0);
        }
    }

    return it;
}

static abtk_tree_t *_abtk_option_find_value(abtk_tree_t *key,size_t index)
{
    abtk_tree_t *it = NULL;
    size_t chk = 0;

    assert(key != NULL);

    it = abtk_tree_child(key,1);
    while(it)
    {
        if (chk++ == index)
            break;

        it = abtk_tree_sibling(it,0);
    }

    return it;
}

static size_t _abtk_option_count_value(abtk_tree_t *key)
{
    abtk_tree_t *it = NULL;
    size_t chk = 0;

    assert(key != NULL);

    it = abtk_tree_child(key,1);
    while(it)
    {
        chk +=1;

        it = abtk_tree_sibling(it,0);
    }

    return chk;
}

int abtk_option_set(abtk_tree_t *opt, const char *key, const char *value)
{
    abtk_tree_t *it_key = NULL;
    abtk_tree_t *it_val = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _abtk_option_find_key(opt,key,1);
    if(!it_key)
        ABTK_ERRNO_AND_RETURN1(ENOMEM,-1);

    /*
     * 允许没有值。
    */
    if (value == NULL || value[0] == '\0')
        return 0;
    
    it_val = abtk_tree_alloc3(strlen(value) + 1);
    if (!it_val)
        ABTK_ERRNO_AND_RETURN1(ENOMEM, -1);

    strcpy(it_val->alloc->pptrs[ABTK_OPTION_VALUE], value);
    abtk_tree_insert2(it_key, it_val, 0);
    

    return 0;
}

const char* abtk_option_get(abtk_tree_t *opt, const char *key,size_t index,const char* defval)
{
    abtk_tree_t *it_key = NULL;
    abtk_tree_t *it_val = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _abtk_option_find_key(opt,key,0);
    if(!it_key)
        ABTK_ERRNO_AND_RETURN1(EAGAIN,defval);

    it_val = _abtk_option_find_value(it_key,index);
    if(!it_val)
        ABTK_ERRNO_AND_RETURN1(EAGAIN,defval);

    return it_val->alloc->pptrs[ABTK_OPTION_VALUE];
}

int abtk_option_get_int(abtk_tree_t *opt, const char *key, size_t index, int defval)
{
    const char *val = abtk_option_get(opt, key, index, NULL);

    if (!val)
        return defval;

    return atoi(val);
}

long abtk_option_get_long(abtk_tree_t *opt, const char *key,size_t index,long defval)
{
    const char *val = abtk_option_get(opt, key, index, NULL);

    if (!val)
        return defval;

    return atol(val);
}

double abtk_option_get_double(abtk_tree_t *opt, const char *key,size_t index,double defval)
{
    const char *val = abtk_option_get(opt, key, index, NULL);

    if (!val)
        return defval;

    return atof(val);
}

ssize_t abtk_option_count(abtk_tree_t *opt, const char *key)
{
    abtk_tree_t *it_key = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _abtk_option_find_key(opt,key,0);
    if(!it_key)
        ABTK_ERRNO_AND_RETURN1(EAGAIN,-1);

    return _abtk_option_count_value(it_key);
}

int abtk_option_remove(abtk_tree_t *opt, const char *key)
{
    abtk_tree_t *it_key = NULL;

    assert(opt != NULL && key != NULL);

    assert(key[0] != '\0');

    it_key = _abtk_option_find_key(opt,key,0);
    if(!it_key)
        ABTK_ERRNO_AND_RETURN1(EAGAIN,-1);

    abtk_tree_unlink(it_key);
    abtk_tree_free(&it_key);

    return 0;
}

ssize_t abtk_option_fprintf(FILE *fp,abtk_tree_t *opt)
{
    abtk_tree_t *it_key = NULL;
    abtk_tree_t *it_val = NULL;
    ssize_t wsize = 0;
    ssize_t wsize2 = 0;

    assert(fp != NULL && opt != NULL);

    it_key = abtk_tree_child(opt,1);
    while(it_key)
    {
        wsize2 = fprintf(fp, "\n%s\n", it_key->alloc->pptrs[ABTK_OPTION_KEY]);
        if (wsize2 <= 0)
            break;

        wsize += wsize2;

        it_val = abtk_tree_child(it_key,1);
        while(it_val)
        {
            wsize2 = fprintf(fp,"%s\n",it_val->alloc->pptrs[ABTK_OPTION_VALUE]);
            if (wsize2 <= 0)
                break;

            wsize += wsize2;

            it_val = abtk_tree_sibling(it_val,0);  
        }

        it_key = abtk_tree_sibling(it_key,0);
    }

    return wsize;
}

ssize_t abtk_option_snprintf(char* buf,size_t max,abtk_tree_t *opt)
{
    FILE* fp = NULL;
    ssize_t wsize = 0;

    assert(buf != NULL && max >0 && opt);

    fp = fmemopen(buf,max,"w");
    if(!fp)
        return -1;

    wsize = abtk_option_fprintf(fp,opt);

    fclose(fp);
    
    return wsize;
}
