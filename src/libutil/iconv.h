/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_ICONV_H
#define GOOD_UTIL_ICONV_H

#include "general.h"

ssize_t good_iconv(iconv_t cd, const char *src, size_t slen, char *dst, size_t dlen,size_t *remain);

#endif //GOOD_UTIL_ICONV_H