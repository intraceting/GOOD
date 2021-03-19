/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_CRC32_H
#define GOOD_UTIL_CRC32_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>

#include "general.h"
#include "thread.h"

/**
 * 计算CRC32值
 * 
 * @param old 上一轮的值。
 * 
 * @note 宽度(32)，多项式(04C11DB7)，初始值(FFFFFFFF)，结果异或值(FFFFFFFF)，输入值反转(true)，输出值反转(true)。
 * 
*/
uint32_t good_crc32_sum(const void *data,size_t size,uint32_t old);


#endif //GOOD_UTIL_CRC32_H