/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "getargs.h"


void good_getargs(good_tree_t *opt,int argc, char* argv[],const char *prefix)
{
    size_t prefix_len = 0;
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


static ssize_t _good_option_getline(FILE *fp,char **line, size_t *len, size_t *rows, char note)
{
    char *line_p = NULL;
    ssize_t chk = -1;

    while ((chk = getline(line, len, fp)) != -1)
    {
        if (rows)
            (*rows) += 1;

        line_p = *line;

        if (*line_p == '\0' || *line_p == note || iscntrl(*line_p))
            continue;
        else
            break;
    }

    return chk;
}

void good_getargs_fp(good_tree_t *opt,FILE *fp,char note,const char *argv0,const char *prefix)
{
    size_t prefix_len = 0;
    const char *it_key = NULL;
    char *line = NULL;
    size_t len = 0;
    size_t rows = 0;

    assert(opt != NULL && fp != NULL && prefix != NULL);

    assert(prefix[0] != '\0');

    prefix_len = strlen(prefix);
    it_key = prefix;

    if(argv0)
        good_option_set(opt,it_key,argv0);

    while (_good_option_getline(fp,&line, &len, &rows,note) != -1)
    {
        /* 去掉字符串两端所有控制字符。 */
        good_strtrim(line,iscntrl,2);

        if (good_strncmp(line, prefix, prefix_len, 1) != 0)
        {
            good_option_set(opt,it_key, line);
        }
        else
        {
            if(it_key != prefix)
                good_heap_freep((void**)&it_key);

            it_key = good_heap_clone(line, len + 1);
            if(!it_key)
                break;

            good_option_set(opt,it_key,NULL);
        }
    }

    /*不要忘记释放这两块内存，不然可能会有内存泄漏的风险。 */
    if(line)
        free(line);
    if(it_key != prefix)
        good_heap_freep((void**)&it_key);

}

void good_getargs_file(good_tree_t *opt,const char *file,char note,const char *argv0,const char *prefix)
{
    FILE *fp = NULL;

    assert(opt != NULL && file != NULL && prefix != NULL);

    assert(file[0] != '\0' && prefix[0] != '\0');

    fp = fopen(file,"r");
    if(!fp)
        return;

    good_getargs_fp(opt,fp,note,argv0,prefix);

    fclose(fp);
}

void good_getargs_text(good_tree_t *opt,const char *text,size_t len,char note,const char *argv0,const char *prefix)
{
    FILE *fp = NULL;

    assert(opt != NULL && text != NULL && len>0 && prefix != NULL);

    assert(prefix[0] != '\0');

    fp = fmemopen((char*)text,len,"r");
    if(!fp)
        return;

    good_getargs_fp(opt,fp,note,argv0,prefix);

    fclose(fp);
}