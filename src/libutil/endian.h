/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_ENDIAN_H
#define GOOD_UTIL_ENDIAN_H

#include "general.h"

/**
 * 字节序检测
 * 
 * @param big 0 检测是否为小端字节序，!0 检测是否为大端字节序。
 * 
 * @return 0 否，!0 是。
 */
int good_endian_check(int big);

/**
 * 字节序交换。
 * 
 * @return dst
*/
uint8_t* good_endian_swap(uint8_t* dst,int len);

/**
 * 网络字节序转本地字节序。
 * 
 * 如果本地是大端字节序，会忽略。
*/
uint8_t* good_endian_ntoh(uint8_t* dst,int len);

/**
 * @see good_endian_ntoh()
*/
uint16_t good_endian_ntoh16(uint16_t num);

/**
 * @see good_endian_ntoh()
*/
uint32_t good_endian_ntoh32(uint32_t num);

/**
 * @see good_endian_ntoh()
*/
uint64_t good_endian_ntoh64(uint64_t num);

/**
 * 本地字节序转网络字节序。
 * 
 * 如果本地是大端字节序，会忽略。
*/
uint8_t* good_endian_hton(uint8_t* dst,int len);

/**
 * @see good_endian_hton()
*/
uint16_t good_endian_hton16(uint16_t num);

/**
 * @see good_endian_hton()
*/
uint32_t good_endian_hton32(uint32_t num);

/**
 * @see good_endian_hton()
*/
uint64_t good_endian_hton64(uint64_t num);


#endif //GOOD_UTIL_ENDIAN_H