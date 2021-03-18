/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "geometry.h"

double good_geom_p2p_distance(const good_point_t *p1, const good_point_t *p2)
{
    assert(p1 && p2);

    return fabs(sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2) + pow(p1->z - p2->z, 2)));
}

double good_geom_halfline_radian(const good_point_t *p1, const good_point_t *p2, int axis)
{
    double radian;

    assert(p1 && p2);

    /*
     * To upper
    */
    axis = toupper(axis);
    assert((axis == 'X') || (axis == 'Y'));

    if (axis == 'X')
        radian = atan2((p2->y - p1->y), (p2->x - p1->x));
    else if (axis == 'Y')
        radian = atan2((p2->x - p1->x), (p2->y - p1->y));

    return radian;
}

void good_geom_point_move(const good_point_t *p1, double radian, double dist, good_point_t *p2)
{
    assert(p1 && p2);

    p2->x = p1->x + dist * cos(radian);
    p2->y = p1->y + dist * sin(radian);
}