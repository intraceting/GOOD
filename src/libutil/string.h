/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_STRING_H
#define GOOD_UTIL_STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <fnmatch.h>

#include "general.h"

/**
 * 字符串查找。
 * 
 * @param caseAb 0 不区分大小写，!0 区分大小写。
 * 
 * @return NULL(0) 未找到，!NULL(0) 匹配到的首地址。
 * 
 * @see strstr()
 * @see strcasestr()
*/
const char* good_strstr(const char *str, const char *sub,int caseAb);

/**
 * 字符串比较。
 * 
 * @param caseAb 0 不区分大小写，!0 区分大小写。
 * 
 * @see strcmp()
 * @see strcasecmp()
*/
int good_strcmp(const char *s1, const char *s2,int caseAb);

/**
 * 字符串比较。
 * 
 * @param caseAb 0 不区分大小写，!0 区分大小写。
 * 
 * @see strncmp()
 * @see strncasecmp()
*/
int good_strncmp(const char *s1, const char *s2,size_t len,int caseAb);

/**
 * 字符串修剪。
 * 
 * @param isctype_cb 字符比较函数。NULL(0) 等于isblank()。返回 !0 是，0 否。
 * @param where 0 右端，1 左端，2 两端。
 * 
 * @see is*() functions. in ctype.h
*/
char* good_strtrim(char* str,int (*isctype_cb)(int c),int where);

/**
 * 字符串分割。
 * 
 * @param str 待分割的字符串。可能会被修改。
 * @param delim 分割字符串。全字匹配，并区分大小写。
 * @param saveptr 临时指针。不可支持访问。
 * 
 * @return NULL(0) 结束，!NULL(0) 分割后的字符串首地址。
 * 
 * @see good_strstr()
*/
char *good_strtok(char *str, const char *delim, char **saveptr);

/**
 * 字符串匹配。
 * 
 * @return 0 成功，!0 失败。
 * 
 * @see fnmatch()
*/
int good_fnmatch(const char *str,const char *wildcard,int caseAb,int ispath);

/**
 * 检测字符串中的字符类型。
 * 
 * @param isctype_cb 字符比较函数。返回 !0 是，0 否。
 * 
 * @return !0 通过，0 未通过。
 * 
 * @see is*() functions. in ctype.h
*/
int good_strtype(const char* str,int (*isctype_cb)(int c));

#endif //GOOD_UTIL_STRING_H