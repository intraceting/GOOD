/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "abcdkutil/general.h"
#include "abcdkutil/getargs.h"
#include "abcdkutil/ffmpeg.h"
#include "abcdkutil/bmp.h"
#include "abcdkutil/freeimage.h"

void test_log(abcdk_tree_t *args)
{
    abcdk_openlog(NULL,LOG_DEBUG,1);

    for(int i = LOG_EMERG ;i<= LOG_DEBUG;i++)
        syslog(i,"haha-%d",i);
}

void test_ffmpeg(abcdk_tree_t *args)
{

#if defined(AVUTIL_AVUTIL_H) && defined(SWSCALE_SWSCALE_H)

    for(int i = 0;i<1000;i++)
    {
        enum AVPixelFormat pixfmt = (enum AVPixelFormat)i;

        if(!ABCDK_AVPIXFMT_CHECK(pixfmt))
            continue;

        int bits = abcdk_av_image_pixfmt_bits(pixfmt,0);
        int bits_pad = abcdk_av_image_pixfmt_bits(pixfmt,1);
        const char *name = abcdk_av_image_pixfmt_name(pixfmt);

        printf("%s(%d): %d/%d bits.\n",name,i,bits,bits_pad);
    }

    
    abcdk_av_image_t src = {AV_PIX_FMT_YUV420P,{NULL,NULL,NULL,NULL},{0,0,0,0},1920,1080};
    abcdk_av_image_t dst = {AV_PIX_FMT_YUV420P,{NULL,NULL,NULL,NULL},{0,0,0,0},1920,1080};
    abcdk_av_image_t dst2 = {AV_PIX_FMT_BGR32,{NULL,NULL,NULL,NULL},{0,0,0,0},1920,1080};

    int src_heights[4]={0}, dst_heights[4]={0}, dst2_heights[4]={0};


    abcdk_av_image_fill_heights(src_heights,src.height,src.pixfmt);
    abcdk_av_image_fill_heights(dst_heights,dst.height,dst.pixfmt);
    abcdk_av_image_fill_heights(dst2_heights,dst2.height,dst2.pixfmt);

    abcdk_av_image_fill_strides2(&src,16);
    abcdk_av_image_fill_strides2(&dst,10);
    abcdk_av_image_fill_strides2(&dst2,1);

    void *src_buf = abcdk_heap_alloc(abcdk_av_image_size3(&src));
    void *dst_buf = abcdk_heap_alloc(abcdk_av_image_size3(&dst));
    void *dst2_buf = abcdk_heap_alloc(abcdk_av_image_size3(&dst2));

    abcdk_av_image_fill_pointers2(&src,src_buf);
    abcdk_av_image_fill_pointers2(&dst,dst_buf);
    abcdk_av_image_fill_pointers2(&dst2,dst2_buf);

    abcdk_av_image_copy2(&dst,&src);

    struct SwsContext *ctx = abcdk_sws_alloc2(&src,&dst2,0);

    int h = sws_scale(ctx,(const uint8_t *const *)src.datas,src.strides,0,src.height,dst2.datas,dst2.strides);
    //int h = sws_scale(ctx,(const uint8_t *const *)src.datas,src.strides,100,src.height,dst2.datas,dst2.strides);

    printf("h = %d\n",h);

    uint8_t *tmp = dst2.datas[0];
    for (int i = 0; i < dst2.height; i++)
    {
        for (int j = 0; j < dst2.width*4; j += 4)
        {
            tmp[j+0] = 0;
            tmp[j+1] = 0;
            tmp[j+2] = 255;
        }

        tmp += dst2.strides[0];
    }

    int chk = abcdk_bmp_save2("/tmp/test_bmp.bmp",dst2.datas[0],dst2.strides[0],dst2.width,dst2.height,32);
    assert(chk==0);

    
    abcdk_sws_free(&ctx);

    abcdk_heap_free(src_buf);
    abcdk_heap_free(dst_buf);
    abcdk_heap_free(dst2_buf);

#endif //AVUTIL_AVUTIL_H && SWSCALE_SWSCALE_H




}

void test_bmp(abcdk_tree_t *args)
{
    const char *src_file = abcdk_option_get(args,"--src-file",0,"");
    const char *dst_file = abcdk_option_get(args,"--dst-file",0,"");

    uint32_t stride = 0;
    uint32_t width = 0;
    int32_t height = 0;
    uint8_t bits = 0;
    int chk = abcdk_bmp_load2(src_file, NULL, 0, 13, &stride, &width, &height, &bits);
    assert(chk == 0);

    printf("s=%u,w=%u,h=%d,b=%hhu\n",stride,width,height,bits);

    uint8_t *data = abcdk_heap_alloc(stride*height);

    chk = abcdk_bmp_load2(src_file, data, stride*height, 1, &stride, &width, &height, &bits);
    assert(chk == 0);


    chk = abcdk_bmp_save2(dst_file, data, stride, width, height, bits);
    assert(chk == 0);


    abcdk_heap_free(data);
    
}

void test_freeimage(abcdk_tree_t *args)
{
#ifdef FREEIMAGE_H

    abcdk_fi_init(1);
    abcdk_fi_init(1);//test run once.

    const char *src_file = abcdk_option_get(args,"--src-file",0,"");
    const char *dst_file = abcdk_option_get(args,"--dst-file",0,"");

    uint32_t stride = 0;
    uint32_t width = 0;
    int32_t height = 0;
    uint8_t bits = 0;
    int chk = abcdk_bmp_load2(src_file, NULL, 0, 13, &stride, &width, &height, &bits);
    assert(chk == 0);

    printf("s=%u,w=%u,h=%d,b=%hhu\n",stride,width,height,bits);

    uint8_t *data = abcdk_heap_alloc(stride*height);

    chk = abcdk_bmp_load2(src_file, data, stride*height, 1, &stride, &width, &height, &bits);
    assert(chk == 0);


    chk = abcdk_fi_save2(FIF_JPEG,JPEG_QUALITYGOOD,dst_file, data, stride, width, height, bits);
    assert(chk == 0);


    abcdk_heap_free(data);


    abcdk_fi_uninit();
    abcdk_fi_uninit();//test run once.

#endif //FREEIMAGE_H
}

int main(int argc, char **argv)
{
    abcdk_openlog(NULL,LOG_DEBUG,1);

    abcdk_tree_t *args = abcdk_tree_alloc3(1);

    abcdk_getargs(args,argc,argv,"--");

    const char *func = abcdk_option_get(args,"--func",0,"");

    if(abcdk_strcmp(func,"test_ffmpeg",0)==0)
        test_ffmpeg(args);

    if(abcdk_strcmp(func,"test_bmp",0)==0)
        test_bmp(args);

    if(abcdk_strcmp(func,"test_freeimage",0)==0)
        test_freeimage(args);


    abcdk_tree_free(&args);
    
    return 0;
}