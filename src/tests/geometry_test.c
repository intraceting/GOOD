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
#include "libutil/ndarray.h"



void test1()
{
    
    good_point_t p1 = {0,0,0};

    good_point_t p2= {95,55,0};

    double d = good_geom_p2p_distance(&p1,&p2);

    printf("d=%lf\n",d);

    double r = good_geom_halfline_radian(&p1,&p2,'X');
    double a = r*180/M_PIl;
    printf("r=%lf;a=%lf\n",r,a);

    good_point_t p3 = {0,0,0};

    good_geom_point_move(&p1,r,d/2,&p3);

    printf("x = %lf y = %lf\n",p3.x,p3.y);

    double d2 = good_geom_p2p_distance(&p1,&p3);

    printf("d2=%lf\n",d2);
}

void test2()
{
    good_point_t p = {0,0,0};

    //good_point_t p1 = {99,95,0};
    //good_point_t p2 = {90,90,0};

    good_point_t p1 = {90,90,0};
    good_point_t p2 = {81,85,0};

    double d1 = good_geom_p2p_distance(&p,&p1);
    double d2 = good_geom_p2p_distance(&p,&p2);
    double r1 = good_geom_halfline_radian(&p,&p1,'X');
    double r2 = good_geom_halfline_radian(&p,&p2,'X');
    double R1 = r1*180/M_PIl;
    double R2 = r2*180/M_PIl;

    printf("d1(%lf)-d2(%lf)=%lf\n",d1,d2,d1-d2);
    printf("r1(%lf)-r2(%lf)=%lf\n",r1,r2,r1-r2);
    printf("R1(%lf)-R2(%lf)=%lf\n",R1,R2,R1-R2);
}

void GuestLocation(const good_point_t *p1,const good_point_t *p2,double xspeed,good_point_t *p3)
{
    double d = good_geom_p2p_distance(p1,p2);
    double r = good_geom_halfline_radian(p1,p2,'X');
    good_geom_point_move(p2,r,d*xspeed,p3);
}

void test3()
{
    good_point_t p1 = {45,45,0};
    good_point_t p2 = {0,0,0};
    good_point_t p3 = {0,0,0};

    GuestLocation(&p1,&p2,1,&p3);
    printf("p1={%lf,%lf},p2={%lf,%lf},p3={%lf,%lf}\n",p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);

    GuestLocation(&p1,&p2,2,&p3);
    printf("p1={%lf,%lf},p2={%lf,%lf},p3={%lf,%lf}\n",p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);

    GuestLocation(&p1,&p2,3,&p3);
    printf("p1={%lf,%lf},p2={%lf,%lf},p3={%lf,%lf}\n",p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);

    GuestLocation(&p1,&p2,0.1,&p3);
    printf("p1={%lf,%lf},p2={%lf,%lf},p3={%lf,%lf}\n",p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);


}

void test4()
{
    good_ndarray_t s = {0};
    good_ndarray_t d = {0};

    s.format = GOOD_NDARRAY_NCHW;
    s.blocks = 3;
    s.width = 16;
    s.height = 16;
    s.depth = 3;
    s.data_bytes = 4;

    d.format = GOOD_NDARRAY_NHWC;
    d.blocks = 3;
    d.width = 16;
    d.height = 16;
    d.depth = 3;
    d.data_bytes = 4;

    s.width_pitch = good_ndarray_width_pitch(&s,5);
    d.width_pitch = good_ndarray_width_pitch(&d,60);

    s.ptr = good_heap_alloc(good_ndarray_area_size(&s));
    d.ptr = good_heap_alloc(good_ndarray_area_size(&d));

    for (size_t n = 0; n < s.blocks; n++)
    {
        for (size_t z = 0; z < s.depth; z++)
        {
            for (size_t y = 0; y < s.height; y++)
            {
                for (size_t x = 0; x < s.width; x++)
                {
                    size_t f = good_ndarray_offset(&s, n, x, y, z);

                    *GOOD_PTR2PTR(int, s.ptr, f) = f/4+1;
                }
            }
        }
    }

    good_ndarray_copy_param p = {0};

    p.blocks = d.blocks;
    p.width = d.width;
    p.height = d.height;
    p.depth = d.depth;

    good_ndarray_copy(&d,&s,&p);

    for (size_t n = 0; n < p.blocks; n++)
    {
        
            
#if 0
                for (size_t x = 0; x < p.width; x++)
                {
                    size_t df = good_ndarray_offset(&d, n, x, y, z);
                    size_t sf = good_ndarray_offset(&s, n, x, y, z);

                    int a = *GOOD_PTR2PTR(int, d.ptr, df);
                    int b = *GOOD_PTR2PTR(int, s.ptr, sf);

               //     printf("[%ld,%ld,%ld,%ld]:%d,%d\n",n,z,y,x,a,b);

                    assert( a==b );
                }
#else
        #if 0
            for (size_t z = 0; z < p.depth; z++)
            {
            size_t sf = good_ndarray_offset(&s, n, 0, 0, z);
            for (size_t y = 0; y < p.height; y++)
            {
                for (size_t x = 0; x < p.width; x++)
                {

                    int b = *GOOD_PTR2PTR(int, s.ptr, sf + y *s.width_pitch + x * s.data_bytes);
                    printf("%d,", b);

                }
                printf("\n");
            }
            #else 
            for (size_t y = 0; y < p.height; y++)
            {
                size_t df = good_ndarray_offset(&d, n, 0, y, 0);

                for (size_t x = 0; x < p.width; x++)
                {
                    for (size_t z = 0; z < p.depth; z++)
                    {
                        int a = *GOOD_PTR2PTR(int, d.ptr, df + x * d.depth * d.data_bytes + z * d.data_bytes);
                        printf("%d,", a);
                    }
                    
                }

                printf("\n");
            }
        #endif 
#endif
        
    }


}

int main(int argc, char **argv)
{
    //test1();
    //test2();

 //   test3();

    test4();


    return 0;
}