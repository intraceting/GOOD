/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_MACRO_H
#define GOOD_UTIL_MACRO_H

/**
 * 转换指针类型。
 * 
 * @note 支持针地址偏移(Bytes)。
 * 
*/
#define GOOD_PTR2PTR(TYPE,VOIDPTR,OFFSET)   ((TYPE*)(((char*)(VOIDPTR))+(OFFSET)))


/**
 * 数值比较，返回最大值。
 * 
*/
#define GOOD_MAX(a,b)   (((a)>(b))?(a):(b))

/**
 * 数值比较，返回最小值。
 * 
*/
#define GOOD_MIN(a,b)   (((a)<(b))?(a):(b))

/**
 * 交换两个数值变量的值。
 * 
*/
#define GOOD_INTEGER_SWAP(A,B)  ({(A)^=(B);(B)^=(A);(A)^=(B);})



#endif //GOOD_UTIL_MACRO_H