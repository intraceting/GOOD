/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#include "html.h"

abcdk_tree_t *_abcdk_html_tag_parse(const char *b ,const char* e)
{
    abcdk_tree_t *tag = NULL;

    const char *tmp = NULL;
    const char *name_b = NULL;
    const char *attr_b = NULL;

    tag = abcdk_tree_alloc2(NULL,3,0);
    if(!tag)
        goto final;

    /*Copy begin ptr.*/
    tmp = b;

    /*Seek name begin ptr.*/
    name_b = ++tmp;

    /*Find name end ptr.*/
    for (; tmp < e; tmp++)
    {
        if(*tmp == ' ' || *tmp == '/' || *tmp == '>')
            break;
    }

    /*Copy name data.*/
    tag->alloc->pptrs[ABCDK_HTML_KEY] = (uint8_t*)abcdk_heap_clone(name_b, tmp - name_b);
    if(!tag->alloc->pptrs[ABCDK_HTML_KEY])
        goto final;

    /*May be tag EOD.*/
    if(*tmp == '/' || *tmp == '>')
        goto final;

    /*Seek attr begin ptr.*/
    attr_b = tmp++;

    /*Find attr end ptr.*/
    for (; tmp < e; tmp++)
    {
        if(*tmp == '/' || *tmp == '>')
            break;
    }

    /*Copy attr data.*/
    tag->alloc->pptrs[ABCDK_HTML_ATTR] = (uint8_t*)abcdk_heap_clone(attr_b, tmp - attr_b);
    if(!tag->alloc->pptrs[ABCDK_HTML_ATTR])
        goto final;

final:

    return tag;
}

void _abcdk_html_parse_real(abcdk_tree_t *root, const char *text)
{
    abcdk_tree_t *tag = NULL;
    const char *b = NULL, *e = NULL;
    const char *tmp = NULL;

    /**/
    tmp = text;

    while (tmp && *tmp)
    {
        b = abcdk_strstr(tmp,"<",1);
        if(!b)
            break;

        if (abcdk_strncmp(b, "<!--", 4,1) == 0)
        {
            b = abcdk_strstr_eod(b,"-->",1);
        }
        else
        {
            e = abcdk_strstr_eod(b,">",1);
            if(!e)
                break;

            tag = _abcdk_html_tag_parse(b,e);
            if(!tag)
                break;

            /**/

            abcdk_tree_insert2(root,tag,0);
            
        }
    }
}

abcdk_tree_t *abcdk_html_parse_text(const char *text)
{
    abcdk_tree_t *root = NULL;

    assert(text != NULL);

    root = abcdk_tree_alloc3(1);
    if(!root)
        goto final;

    _abcdk_html_parse_real(root,text);

final:

    return root;
}

abcdk_tree_t *abcdk_html_parse_file(const char *file)
{
    abcdk_tree_t *root = NULL;
    abcdk_allocator_t *fmem = NULL;

    assert(file != NULL);

    fmem = abcdk_mmap2(file,0,0);
    if(!fmem)
        goto final;

    root = abcdk_html_parse_text((char*)fmem->pptrs[0]);

final:

    abcdk_allocator_unref(&fmem);

    return root;
}