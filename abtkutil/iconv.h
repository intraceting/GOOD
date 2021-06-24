/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_ICONV_H
#define ABTKUTIL_ICONV_H

#include "general.h"

/**
 * 编码转换
 * 
 * @param remain 未被转换数据长度的指针，NULL(0) 忽略。
 * 
 * @return >= 0 转换成功的数据长度(Bytes)，-1 失败。
*/
ssize_t abtk_iconv(iconv_t cd, const char *src, size_t slen, char *dst, size_t dlen,size_t *remain);

#endif //ABTKUTIL_ICONV_H