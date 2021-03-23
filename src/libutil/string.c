/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "string.h"

const char *good_strstr(const char *str, const char *sub, int caseAb)
{
    assert(str != NULL && sub != NULL);

    if (caseAb)
        return strstr(str, sub);

    return strcasestr(str, sub);
}

int good_strcmp(const char *s1, const char *s2, int caseAb)
{
    assert(s1 != NULL && s2 != NULL);

    if (caseAb)
        return strcmp(s1, s2);

    return strcasecmp(s1, s2);
}

int good_strncmp(const char *s1, const char *s2, size_t len, int caseAb)
{
    assert(s1 != NULL && s2 != NULL && len > 0);

    if (caseAb)
        return strncmp(s1, s2, len);

    return strncasecmp(s1, s2, len);
}

char *good_strtrim(char *str, int (*isctype_cb)(int c),int where)
{
    char *tmp = NULL;
    size_t len = 0;
    size_t blklen = 0;

    assert(str);

    if (!isctype_cb)
        isctype_cb = isblank;

    tmp = str;
    len = strlen(str);

    if (0 == where)
    {
        while (*tmp)
            tmp++;

        while (isctype_cb(*(--tmp)) != 0)
            *tmp = '\0';
    }
    else if (1 == where)
    {
        while (isctype_cb(*(tmp)) != 0)
        {
            tmp++;
            blklen++;
        }

        for (size_t i = 0; i < len - blklen; i++)
            str[i] = str[i + blklen];

        for (size_t j = len - blklen; j < len; j++)
            str[j] = '\0';
    }
    else if (2 == where)
    {
        good_strtrim(str,isctype_cb,0);
        good_strtrim(str,isctype_cb,1);
    }

    return str;
}

char *good_strtok(char *str,const char *delim, char **saveptr)
{
    char* prev = NULL;
    char* find = NULL;

    assert(str && delim && saveptr);

    if(*saveptr)
        prev = *saveptr;
    else 
        prev = str;

    find = (char *)good_strstr(prev, delim, 1);
    if (find)
    {
        *find = '\0';
        *saveptr = find + strlen(delim);
    }
    else if (*prev != '\0')
    {
        *saveptr = prev + strlen(prev);
    }
    else
    {
        prev = NULL;
        *saveptr = NULL;
    }

    return prev;
}

int good_strmatch(const char *str,const char *wildcard,int caseAb,int ispath)
{
    int flag = 0;
    int chk = FNM_NOMATCH;

    assert(str && wildcard);

    if (caseAb)
        flag |= FNM_CASEFOLD;
    if (ispath)
        FNM_PATHNAME | FNM_PERIOD;

    chk = fnmatch(wildcard, str, flag);

    return ((chk==FNM_NOMATCH)?-1:0);
}