/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "file.h"

char *good_path_concat(char *path, const char *suffix)
{
    size_t len = 0;

    assert(path != NULL && suffix != NULL);

    len = strlen(path);
    if (len > 0)
    {
        if ((path[len - 1] == '/') && (suffix[0] == '/'))
        {
            path[len - 1] = '\0';
            len -= 1;
        }
        else if ((path[len - 1] != '/') && (suffix[0] != '/'))
        {
            path[len] = '/';
            len += 1;
        }
    }

    /*
     * 要有足够的可用空间，不然会溢出。
    */
    strcat(path + len, suffix);

    return path;
}

void good_path_create(const char *path, mode_t mode)
{
    size_t len = 0;
    char *tmp = NULL;
    int chk = 0;

    assert(path != NULL);

    len = strlen(path);
    if (len <= 0)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    tmp = (char *)good_heap_dup(path, len + 1);
    if (!tmp)
        GOOD_ERRNO_AND_RETURN0(ENOMEM);

    /*
     * 必须允许当前用户具有读、写、访问权限。
    */
    mode |= S_IRWXU;

    for (size_t i = 1; i < len; i++)
    {
        if (tmp[i] != '/')
            continue;

        tmp[i] = '\0';

        if (access(tmp, F_OK) != 0)
            chk = mkdir(tmp, mode & (S_IRWXU | S_IRWXG | S_IRWXO));

        tmp[i] = '/';

        if (chk != 0)
            break;
    }

    if (tmp)
        good_heap_freep((void**)&tmp);
}

char *good_path_dirname(char *dst, const char *src)
{
    char *find = NULL;
    char *path = NULL;

    assert(dst != NULL && src != NULL);

    path = (char *)good_heap_dup(src, strlen(src) + 1);
    if (!path)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, NULL);

    find = dirname(path);
    if (find)
        memcpy(dst, find, strlen(find) + 1);

    good_heap_freep((void**)&path);

    return dst;
}

char *good_path_basename(char *dst, const char *src)
{
    char *find = NULL;
    char *path = NULL;

    assert(dst != NULL && src != NULL);

    path = (char *)good_heap_dup(src, strlen(src) + 1);
    if (!path)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, NULL);

    find = basename(path);
    if (find)
        memcpy(dst, find, strlen(find) + 1);

    good_heap_freep((void**)&path);

    return dst;
}

char *good_path_prettify(char *dst, const char *src)
{
    char* s = NULL;
    char* d = NULL;
    char* t = NULL;
    char* saveptr = NULL;

    assert(dst != NULL && src != NULL);

    s = good_heap_dup(src,strlen(src));
    d = dst;

    if(!s)
        GOOD_ERRNO_AND_RETURN1(ENOMEM,NULL);

    while(1)
    {
        t = good_strtok(s,"/",&saveptr);
        if(!t)
            break;
        
        if(*t == '\0')
            continue;

        if(good_strncmp(t,".",1,1)==0)
            continue;

        if(good_strncmp(t,"..",2,1)==0)
        {
            while (d != dst && *(d - 1) != '/')
                *d-- = '\0';
        }
        else
        {
            if(d != dst || *(d - 1) != '/')
                *d++ = '/';

            while (*t)
                *d++ = *t++;
        }
    }

    good_heap_freep((void**)&s);

    return dst;
}