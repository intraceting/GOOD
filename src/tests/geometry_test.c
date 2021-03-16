/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "libutil/geometry.h"


int main(int argc, char **argv)
{
    
    good_point_t p1 = {0,0,0};

    good_point_t p2= {1,1,0};

    double d = good_geom_p2p_distance(&p1,&p2);

    printf("d=%lf\n",d);

    double pi = 3.1415926;

    double r = good_geom_halfline_radian(&p1,&p2,'X');
    double a = r*180/pi;
    printf("r=%lf;a=%lf\n",r,a);

    good_point_t p3 = {0,0,0};

    good_geom_point_move(&p1,r,d,&p3);

    printf("x = %lf y = %lf\n",p3.x,p3.y);

    double d2 = good_geom_p2p_distance(&p1,&p3);

    printf("d2=%lf\n",d2);

    return 0;
}