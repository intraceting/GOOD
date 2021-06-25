/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKUTIL_GEOMETRY_H
#define ABCDKUTIL_GEOMETRY_H

#include "general.h"

/**
 * 点
 * 
*/
typedef struct _abcdk_point
{
    /**
     * @brief X 
     * 
     */
    double x;

    /**
     * @brief Y
     * 
     */
    double y;

    /**
     * @brief Z
     * 
     */
    double z;
} abcdk_point_t;

/**
 * 计算空间两点之间的距离。
 *  
 */
double abcdk_geom_p2p_distance(const abcdk_point_t *p1, const abcdk_point_t *p2);

/**
 * 计算射线弧度。
 * 
 * Z轴忽略。
 * 
 * 角度=弧度*180/PI(3.1415926...)。
 * 
 * @param axis 仅支持X轴或者Y轴。
 * 
*/
double abcdk_geom_halfline_radian(const abcdk_point_t *p1, const abcdk_point_t *p2, int axis);

/**
 * 点移动。
 * 
 * Z轴忽略。
 * 
 * @param p1 
 * @param radian 弧度
 * @param dist 距离
 * @param p2 
 */
void abcdk_geom_point_move(const abcdk_point_t *p1,double radian,double dist,abcdk_point_t *p2);

#endif //ABCDKUTIL_GEOMETRY_H