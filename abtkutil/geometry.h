/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_GEOMETRY_H
#define ABTKUTIL_GEOMETRY_H

#include "general.h"

/**
 * 点
 * 
*/
typedef struct _abtk_point
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
} abtk_point_t;

/**
 * 计算空间两点之间的距离。
 *  
 */
double abtk_geom_p2p_distance(const abtk_point_t *p1, const abtk_point_t *p2);

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
double abtk_geom_halfline_radian(const abtk_point_t *p1, const abtk_point_t *p2, int axis);

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
void abtk_geom_point_move(const abtk_point_t *p1,double radian,double dist,abtk_point_t *p2);

#endif //ABTKUTIL_GEOMETRY_H