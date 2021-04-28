/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "ndarray.h"

size_t good_ndarray_width_pitch(const good_ndarray_t *ary, size_t align)
{
    size_t size = 0;

    assert(ary != NULL);

    assert(ary->format == GOOD_NDARRAY_NCHW || ary->format == GOOD_NDARRAY_NHWC);

    if (ary->format == GOOD_NDARRAY_NCHW)
    {
        assert(ary->width > 0 && ary->data_bytes > 0);

        size = good_align(ary->width * ary->data_bytes, align);
    }
    else if (ary->format == GOOD_NDARRAY_NHWC)
    {
        assert(ary->width > 0 && ary->depth > 0 && ary->data_bytes > 0);

        size = good_align(ary->width * ary->depth * ary->data_bytes, align);
    }

    return size;
}

size_t good_ndarray_area_size(const good_ndarray_t *ary)
{
    size_t size = 0;

    assert(ary != NULL);

    assert(ary->format == GOOD_NDARRAY_NCHW || ary->format == GOOD_NDARRAY_NHWC);
    assert(ary->blocks > 0 && ary->height > 0 && ary->width > 0 && ary->depth > 0 && ary->data_bytes > 0);

    assert(ary->width_pitch >= good_ndarray_width_pitch(ary,1));

    if(ary->format == GOOD_NDARRAY_NCHW)
    {
        size = ary->blocks * ary->depth * ary->height * ary->width_pitch;
    }
    else if(ary->format == GOOD_NDARRAY_NHWC)
    {
        size = ary->blocks * ary->height * ary->width_pitch;
    }

    return size;
}

size_t good_ndarray_offset(const good_ndarray_t *ary,size_t n,size_t x,size_t y,size_t z)
{
    size_t size = -1;

    assert(ary != NULL);
    
    assert(ary->format == GOOD_NDARRAY_NCHW || ary->format == GOOD_NDARRAY_NHWC);
    assert(ary->blocks > 0 && ary->height > 0 && ary->width > 0 && ary->depth > 0 && ary->data_bytes > 0);

    assert(n < ary->blocks && x < ary->width && y < ary->height && z < ary->depth);

    assert(ary->width_pitch >= good_ndarray_width_pitch(ary,1));

    if(ary->format == GOOD_NDARRAY_NCHW)
    {
        /*N*深*高*宽步长*/
        size = n * ary->depth * ary->height * ary->width_pitch;
        /*Z*高*宽步长*/
        size += z * ary->height * ary->width_pitch;
        /*Y*宽步长*/
        size += y * ary->width_pitch;
        /*X*元素大小*/
        size += x * ary->data_bytes;
        
    }
    else if(ary->format == GOOD_NDARRAY_NHWC)
    {
        /*n*高*宽步长*/
        size = n * ary->height * ary->width_pitch;
        /*Y*宽步长*/
        size += y * ary->width_pitch;
        /*X*深*元素大小*/
        size += x * ary->depth * ary->data_bytes;
        /*Z*元素大小*/
        size += z * ary->data_bytes;
    }

    return size;
}

void good_ndarray_copy(good_ndarray_t *dst,const good_ndarray_t *src,good_ndarray_copy_param *param)
{
    //size_t dst_offset = -1;
    //size_t src_offset = -1;
    long processors = 1;

    assert(dst != NULL && src != NULL && param != NULL);

    assert(dst->ptr != NULL && src->ptr != NULL);

    assert(dst->format == GOOD_NDARRAY_NCHW || dst->format == GOOD_NDARRAY_NHWC);
    assert(dst->blocks > 0 && dst->height > 0 && dst->width > 0 && dst->depth > 0 && dst->data_bytes > 0);

    assert(src->format == GOOD_NDARRAY_NCHW || src->format == GOOD_NDARRAY_NHWC);
    assert(src->blocks > 0 && src->height > 0 && src->width > 0 && src->depth > 0 && src->data_bytes > 0);

    assert(param->blocks > 0 && param->height > 0 && param->width > 0 && param->depth > 0);

    assert(param->dst_n + param->blocks <= dst->blocks &&
                      param->dst_x + param->width <= dst->width &&
                      param->dst_y + param->height <= dst->height &&
                      param->dst_z + param->depth <= dst->depth);

    assert(param->src_n + param->blocks <= src->blocks &&
                      param->src_x + param->width <= src->width &&
                      param->src_y + param->height <= src->height &&
                      param->src_z + param->depth <= src->depth);

    assert(param->copy_cb == NULL && dst->data_bytes == src->data_bytes);

    processors = sysconf(_SC_NPROCESSORS_ONLN);

    #pragma omp parallel for num_threads(param->blocks)
    for (size_t n = 0; n < param->blocks; n++)
    {
        for (size_t z = 0; z < param->depth; z++)
        {
            for (size_t y = 0; y < param->height; y++)
            {
                for (size_t x = 0; x < param->width; x++)
                {
                    size_t dst_offset = good_ndarray_offset(dst, n + param->dst_n, x + param->dst_x, y + param->dst_y, z + param->dst_z);
                    size_t src_offset = good_ndarray_offset(src, n + param->src_n, x + param->src_x, y + param->src_y, z + param->src_z);

                    if (param->copy_cb)
                    {
                        param->copy_cb(GOOD_PTR2PTR(void, dst->ptr, dst_offset), dst->data_bytes,
                                       GOOD_PTR2PTR(void, src->ptr, src_offset), src->data_bytes,
                                       param->opaque);
                    }
                    else
                    {
                        memcpy(GOOD_PTR2PTR(void, dst->ptr, dst_offset),
                               GOOD_PTR2PTR(void, src->ptr, src_offset),
                               GOOD_MIN(dst->data_bytes, src->data_bytes));
                    }

                    int a = *GOOD_PTR2PTR(int, dst->ptr, dst_offset);
                    int b = *GOOD_PTR2PTR(int, src->ptr, src_offset);

                  //  printf("[%ld,%ld,%ld,%ld]:%d,%d\n",n,z,y,x,a,b);

                    assert( a==b );
                }
            }
        }
    }

}