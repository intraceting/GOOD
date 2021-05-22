/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_CRC32_H
#define GOODUTIL_CRC32_H

#include "general.h"

/**
 * 计算CRC32值。
 * 
 * 宽度(32 bits)，多项式(04C11DB7)，初始值(FFFFFFFF)，结果异或值(FFFFFFFF)，输入值反转(true)，输出值反转(true)。
 * 
 * @param old 上一轮的值。
 * 
*/
uint32_t good_crc32_sum(const void *data,size_t size,uint32_t old);


#endif //GOODUTIL_CRC32_H