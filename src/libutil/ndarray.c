/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "ndarray.h"

size_t good_ndarray_width_pitch(const good_ndarray_attr *attr, size_t align)
{
    size_t size = 0;

    assert(attr != NULL);

    assert(attr->format == GOOD_NDARRAY_NCHW || attr->format == GOOD_NDARRAY_NHWC);

    if (attr->format == GOOD_NDARRAY_NCHW)
    {
        assert(attr->width > 0 && attr->data_bytes > 0);

        size = good_align(attr->width * attr->data_bytes, align);
    }
    else if (attr->format == GOOD_NDARRAY_NHWC)
    {
        assert(attr->width > 0 && attr->depth > 0 && attr->data_bytes > 0);

        size = good_align(attr->width * attr->depth * attr->data_bytes, align);
    }

    return size;
}

size_t good_ndarray_area_size(const good_ndarray_attr *attr)
{
    size_t size = 0;

    assert(attr != NULL);

    assert(attr->format == GOOD_NDARRAY_NCHW || attr->format == GOOD_NDARRAY_NHWC);
    assert(attr->blocks > 0 && attr->height > 0 && attr->width > 0 && attr->depth > 0 && attr->data_bytes > 0);

    assert(attr->width_pitch >= good_ndarray_width_pitch(attr,1));

    if(attr->format == GOOD_NDARRAY_NCHW)
    {
        size = attr->blocks * attr->depth * attr->height * attr->width_pitch;
    }
    else if(attr->format == GOOD_NDARRAY_NHWC)
    {
        size = attr->blocks * attr->height * attr->width_pitch;
    }

    return size;
}

size_t good_ndarray_offset(const good_ndarray_attr *attr,size_t n,size_t x,size_t y,size_t z)
{
    size_t size = -1;
    
    assert(attr->format == GOOD_NDARRAY_NCHW || attr->format == GOOD_NDARRAY_NHWC);
    assert(attr->blocks > 0 && attr->height > 0 && attr->width > 0 && attr->depth > 0 && attr->data_bytes > 0);

    assert(n < attr->blocks && x < attr->width && y < attr->height && z < attr->depth);

    assert(attr->width_pitch >= good_ndarray_width_pitch(attr,1));

    if(attr->format == GOOD_NDARRAY_NCHW)
    {
        /*X*元素大小*/
        size = x * attr->data_bytes;
        /*Y*宽*元素大小*/
        size += y * attr->width * attr->data_bytes;
        /*Z*高*宽步长*/
        size += z * attr->height * attr->width_pitch;
        /*N*深*高*宽步长*/
        size += n * attr->depth * attr->height * attr->width_pitch;
    }
    else if(attr->format == GOOD_NDARRAY_NHWC)
    {
        /*X*深*元素大小*/
        size = x * attr->depth * attr->data_bytes;
        /*Z*元素大小*/
        size += z * attr->data_bytes;
        /*Y*宽步长*/
        size += y * attr->width_pitch;
        /*n*高*宽步长*/
        size += n * attr->height * attr->width_pitch;
    }

    return size;
}