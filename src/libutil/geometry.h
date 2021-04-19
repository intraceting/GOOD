/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_GEOMETRY_H
#define GOOD_UTIL_GEOMETRY_H

#include "general.h"

/**
 * 点
 * 
 */
typedef struct _good_point
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
} good_point_t;

/**
 * 计算空间两点之间的距离。
 *  
 */
double good_geom_p2p_distance(const good_point_t *p1, const good_point_t *p2);

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
double good_geom_halfline_radian(const good_point_t *p1, const good_point_t *p2, int axis);

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
void good_geom_point_move(const good_point_t *p1,double radian,double dist,good_point_t *p2);

#endif //GOOD_UTIL_GEOMETRY_H