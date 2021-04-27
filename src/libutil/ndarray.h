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
 * 多维数组格式。
 * 
*/
typedef enum _good_ndarray_fmt
{
    /*
     * 
     * NHWC(NWCH)格式。
     * 
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
 * 多维数组的属性。
 * 
*/
typedef struct _good_ndarray_attr
{
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
     * 数据大小(bytes)。
    */
    size_t data_bytes;
    
}good_ndarray_attr;


/**
 * 多维数组复制的参数。
 * 
*/
typedef struct _good_ndarray_copy_param
{
    /**
     * 目标数组的属性。
    */
    good_ndarray_attr dst_attr;

    /* 目标起始位置。*/
    size_t dst_n; /** 0 是开始。*/
    size_t dst_x; /** 0 是开始。*/
    size_t dst_y; /** 0 是开始。*/
    size_t dst_z; /** 0 是开始。*/

    /**
     * 源数组的属性。
    */
    good_ndarray_attr src_attr;

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

} good_ndarray_copy_param;

/**
 * 计算多维数组宽度对齐的大小(bytes)。
*/
size_t good_ndarray_width_pitch(const good_ndarray_attr *attr, size_t align);

/**
 * 计算多维数组占用空间的大小(bytes)。
*/
size_t good_ndarray_area_size(const good_ndarray_attr *attr);

/**
 * 计算多维数组坐标点内存偏移量(bytes)。
 * 
 * @param n 块的坐标，0 是开始。
 * @param x 宽的坐标，0 是开始。
 * @param y 高的坐标，0 是开始。
 * @param z 深的坐标，0 是开始。
 * 
*/
size_t good_ndarray_offset(const good_ndarray_attr *attr,size_t n,size_t x,size_t y,size_t z);

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
void good_ndarray_copy(void *dst,const void *src,good_ndarray_copy_param *param);

#endif //GOOD_UTIL_NDARRAY_H