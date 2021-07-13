/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#include "robots.h"

int _abcdk_robots_check_agent(const char *line, const char *agent)
{
    const char *b = NULL;
    int chk = -1;

    chk = abcdk_strncmp(line,"User-agent",9,0);
    if(chk!=0)
        ABCDK_ERRNO_AND_GOTO1(EPERM, final);

    b = line;

    for (;b++;)
    {
        if (*b == '\0')
            ABCDK_ERRNO_AND_GOTO1(EPERM, final);

        if (*b == ':')
            break;
    }

    if (*b == ':')
        b += 1;

    for (;b++;)
    {
        if (*b == '\0')
            ABCDK_ERRNO_AND_GOTO1(EPERM, final);

        if(!isspace(*b))
            break;
    }

    chk = abcdk_strcmp(b,agent,0);

final:

    return chk;
}

abcdk_tree_t *_abcdk_robots_parse_rule(const char *line)
{
    abcdk_tree_t *rule = NULL;
    const char *b = NULL;
    const char *e = NULL;
    int flag = 0;

    if (abcdk_strncmp(line, "Disallow", 8, 0) == 0)
        flag = 2;
    else if (abcdk_strncmp(line, "Allow", 5, 0) == 0)
        flag = 1;
    else if (abcdk_strncmp(line, "Sitemap", 7, 0) == 0)
        flag = 3;

    /*只支持这三种。*/
    if (flag != 1 && flag != 2 && flag != 3)
        ABCDK_ERRNO_AND_GOTO1(EPERM, final);

    b = line;

    for (;b++;)
    {
        if (*b == '\0')
            ABCDK_ERRNO_AND_GOTO1(EPERM, final);

        if (*b == ':')
            break;
    }

    if (*b == ':')
        b += 1;

    for (;b++;)
    {
        if (*b == '\0')
            ABCDK_ERRNO_AND_GOTO1(EPERM, final);

        if(!isspace(*b))
            break;
    }

    e = b + 1;

    /*查找结束符。*/
    for (; *e != '\0'; e++)
    {
        /*Nothing.*/;
    }

    size_t sizes[2] = {sizeof(uint8_t), (e - b) + 1};

    rule = abcdk_tree_alloc2(sizes,2,0);
    if(!rule)
        goto final;

    ABCDK_PTR2U8(rule->alloc->pptrs[ABCDK_ROBOTS_FLAG], 0) = flag;
    strncpy((char *)rule->alloc->pptrs[ABCDK_ROBOTS_PATH], b, e - b);

final:

    return rule;
}

void _abcdk_robots_parse_real(abcdk_tree_t *root, const char *text, const char *agent)
{
    abcdk_tree_t *rule = NULL;
    abcdk_buffer_t *buf = NULL;
    char *line = NULL;
    size_t size = 16 * PATH_MAX;
    ssize_t size2 = 0;
    int agent_ok = 0;

    buf = abcdk_buffer_alloc(NULL);
    if(!buf)
        return;

    buf->data = (void*)text;
    buf->size = strlen(text);
    buf->wsize = buf->size;
    buf->rsize = 0;

    line = abcdk_heap_alloc(size);
    if(!line)
        goto final;

    for(;;)
    {
        size2 = abcdk_buffer_readline(buf, line, size);
        if (size2 <= 0)
            break;

        /*跳过太长的。*/
        if (size2 == size && line[size2 - 1] != '\n')
            continue;

        /* 去掉字符串两端所有空白字符。 */
        abcdk_strtrim(line, isspace, 2);

        if(!agent_ok)
        {
            agent_ok = (_abcdk_robots_check_agent(line,agent)==0);
        }
        else 
        {
            /*只匹配有效段落。*/
            if (*line == '\0')
                goto final;

            rule = _abcdk_robots_parse_rule(line);
            if(!rule)
                continue;

            /*加入到树的子节点末尾.*/
            abcdk_tree_insert2(root, rule, 0);
        }

    }


final:

    abcdk_heap_free(line);
    abcdk_buffer_free(&buf);
}

abcdk_tree_t *abcdk_robots_parse_text(const char *text,const char *agent)
{
    abcdk_tree_t *root = NULL;

    assert(text != NULL);

    root = abcdk_tree_alloc3(1);
    if (!root)
        goto final;

    _abcdk_robots_parse_real(root, text, agent);

final:

    return root;
}

abcdk_tree_t *abcdk_robots_parse_file(const char *file,const char *agent)
{
    abcdk_tree_t *root = NULL;
    abcdk_allocator_t *fmem = NULL;

    assert(file != NULL);

    fmem = abcdk_mmap2(file, 0, 0);
    if (!fmem)
        goto final;

    root = abcdk_robots_parse_text((char *)fmem->pptrs[0],agent);

final:

    abcdk_allocator_unref(&fmem);

    return root;
}