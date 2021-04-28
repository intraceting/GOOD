/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_NDARRAY_H
#define GOOD_UTIL_NDARRAY_H

#include "general.h"


/**
 * 简单的多维数组。
 * 
*/
typedef struct _good_ndarray
{
    /**
     * 数据的指针。
    */
    void *ptr;

    /**
     * 格式。
    */
    int format;

    /**
     * 块数量。
    */
    size_t blocks;

    /**
     * 高度。
    */
    size_t height;

    /**
     * 宽度。
    */
    size_t width;

    /**
     * 宽度对齐的大小(bytes)。
    */
    size_t width_pitch;

    /**
     * 深度。
    */
    size_t depth;

    /**
     * 元素大小(bytes)。
    */
    size_t data_bytes;
    
}good_ndarray_t;


/**
 * 多维数组的格式。
 * 
*/
typedef enum _good_ndarray_fmt
{
    /**
     * 
     * NHWC格式。
     * 
     * [AB....AB....AB....AB....AB....XXX]
     * [..............................XXX]
     * [AB....AB....AB....AB....AB....XXX]
     * [.................................]
     * [CD....CD....CD....CD....CD....XXX]
     * [..............................XXX]
     * [CD....CD....CD....CD....CD....XXX]
     * 
     * 
    */
    GOOD_NDARRAY_NHWC = 1,
#define GOOD_NDARRAY_NHWC   GOOD_NDARRAY_NHWC

    /**
     * 
     * NCHW格式。
     * 
     * [AAA..........................AXXX]
     * [AAA..........................AXXX]
     * [BBB..........................BXXX]
     * [BBB..........................BXXX]
     * [.................................]
     * [CCC..........................CXXX]
     * [CCC..........................CXXX]
     * [DDD..........................DXXX]
     * [DDD..........................DXXX]
     *
     * 
    */
    GOOD_NDARRAY_NCHW = 2
#define GOOD_NDARRAY_NCHW   GOOD_NDARRAY_NCHW

}good_ndarray_fmt;

/**
 * 多维数组复制的参数。
 * 
*/
typedef struct _good_ndarray_copy_param
{
    /* 目标起始位置。*/
    size_t dst_n; /** 0 是开始。*/
    size_t dst_x; /** 0 是开始。*/
    size_t dst_y; /** 0 是开始。*/
    size_t dst_z; /** 0 是开始。*/

    /* 源起始位置。*/
    size_t src_n; /** 0 是开始。*/
    size_t src_x; /** 0 是开始。*/
    size_t src_y; /** 0 是开始。*/
    size_t src_z; /** 0 是开始。*/

    /*复制的数量。*/
    size_t blocks; /** 1 是开始。*/
    size_t height; /** 1 是开始。*/
    size_t width;  /** 1 是开始。*/
    size_t depth;  /** 1 是开始。*/

    /**
     * 复制函数。
     * 
     * 如果源和目标的元素大小不一样，此函数不能为NULL(0)。
    */
    void (*copy_cb)(void *dst, size_t dst_dbytes, const void *src, size_t src_dbytes, void *opaque);

    /**
    * 环境指针。
    */
    void *opaque;

} good_ndarray_copy_param;

/**
 * 计算多维数组宽度对齐的大小(bytes)。
*/
size_t good_ndarray_width_pitch(const good_ndarray_t *ary, size_t align);

/**
 * 计算多维数组占用空间的大小(bytes)。
*/
size_t good_ndarray_area_size(const good_ndarray_t *ary);

/**
 * 计算多维数组坐标点内存偏移量(bytes)。
 * 
 * @param n 块的坐标，0 是开始。
 * @param x 宽的坐标，0 是开始。
 * @param y 高的坐标，0 是开始。
 * @param z 深的坐标，0 是开始。
*/
size_t good_ndarray_offset(const good_ndarray_t *ary,size_t n,size_t x,size_t y,size_t z);

/** 
 * 多维数组的复制。
 * 
 * @warning 如果元素大小不一样，复制时会截断。
 * 
 * @param dst 目标的指针。
 * @param src 源的指针。
 * @param param 参数的指针。
 * 
*/
void good_ndarray_copy(good_ndarray_t *dst,const good_ndarray_t *src,good_ndarray_copy_param *param);

#endif //GOOD_UTIL_NDARRAY_H