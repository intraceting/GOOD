/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_GENERAL_H
#define GOOD_UTIL_GENERAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

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

/*
 * 设置出错码，并返回。
*/
#define GOOD_ERRNO_AND_RETURN0(E) ({errno=(E);return;})

/*
 * 设置出错码，并返回值。
*/
#define GOOD_ERRNO_AND_RETURN1(E, V) ({errno=(E);return (V);})

/**
 * 对齐
 * 
*/
size_t good_align(size_t size,size_t align);

/**
 * 执行一次。
 * 
 * @param status 状态，一般是静态类型。必须初始化为0。
 * @param routine 执行函数。0 成功，!0 失败。
 * 
 * @return 0 第一次，1 第N次，-1 失败。
 * 
 * @see pthread_once()
*/
int good_once(atomic_int* status,int (*routine)(void *opaque),void *opaque);


#endif //GOOD_UTIL_GENERAL_H