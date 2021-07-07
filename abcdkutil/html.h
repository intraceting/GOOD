/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKCOMM_HTML_H
#define ABCDKCOMM_HTML_H

#include "tree.h"
#include "mman.h"

__BEGIN_DECLS

/**
 * HTML的字段。
*/
enum _abcdk_html_field
{
    /** Key 字段索引。*/
   ABCDK_HTML_KEY = 0,
#define ABCDK_HTML_KEY  ABCDK_HTML_KEY

    /** Value 字段索引。*/
   ABCDK_HTML_VALUE = 1,
#define ABCDK_HTML_VALUE    ABCDK_HTML_VALUE

    /** Attribute 字段索引。*/
   ABCDK_HTML_ATTR = 2
#define ABCDK_HTML_ATTR    ABCDK_HTML_ATTR

};

/**
 * 
*/
abcdk_tree_t *abcdk_html_parse_text(const char *text);

/**
 * 
*/
abcdk_tree_t *abcdk_html_parse_file(const char *file);


__END_DECLS

#endif //ABCDKCOMM_HTML_H