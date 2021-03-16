/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_MACRO_H
#define GOOD_UTIL_MACRO_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/**
 * 转换指针类型。
 * 
 * @note 支持针地址偏移(Bytes)。
 * 
*/
#define GOOD_PTR2PTR(T,PTR,OF)   ((T*)(((char*)(PTR))+(OF)))

/**
 * 数值比较，返回最大值。
 * 
*/
#define GOOD_MAX(A,B)   (((A)>(B))?(A):(B))

/**
 * 数值比较，返回最小值。
 * 
*/
#define GOOD_MIN(A,B)   (((A)<(B))?(A):(B))

/**
 * 交换两个数值变量的值。
 * 
*/
#define GOOD_INTEGER_SWAP(A,B)  ({(A)^=(B);(B)^=(A);(A)^=(B);})

/**
 * 尺寸对齐
 * 
*/
size_t good_size_align(size_t size,size_t align);

/**
 * BKDR32
 * 
 */
uint32_t good_hash_bkdr(const void* data,size_t size);

/**
 * BKDR64
 * 
 */
uint64_t good_hash_bkdr64(const void* data,size_t size);

/**
 * 字节序检测
 * 
 * @param big 0 检测是否为小端字节序，!0 检测是否为大端字节序。
 * 
 * @return 0 否，!0 是。
 */
int good_endian_check(int big);

/**
 * 字节序交换
 * 
 * @return dst
*/
uint8_t* good_endian_swap(uint8_t* dst,int len);

/**
 * 网络字节序转本地字节序
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
 * 本地字节序转网络字节序
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

/**
 * 内存申请
 * 
 * @see calloc()
 * 
 */
void* good_heap_alloc(size_t size);

/**
 * 内存申请
 * 
 * @see realloc()
 * 
 */
void* good_heap_realloc(void *buf,size_t size);

/**
 * 内存释放
 * 
 * @param data 
 * 
 * @see free()
 */
void good_heap_free(void *data);

/**
 * 内存释放，并清空指针。
 * 
 * @param data 指针的指针
 * 
 * @see good_heap_free()
 */
void good_heap_freep(void **data);

#endif //GOOD_UTIL_MACRO_H