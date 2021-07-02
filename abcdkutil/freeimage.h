/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKUTIL_FREEIMAGE_H
#define ABCDKUTIL_FREEIMAGE_H

#include "general.h"

#ifdef HAVE_FREEIMAGE
#include <FreeImage.h>
#endif //HAVE_FREEIMAGE

__BEGIN_DECLS

#ifdef FREEIMAGE_H

/** 检查图像格式是否支持。*/
#define ABCDK_FI_IMGFMT_CHECK(fifmt)   ((fifmt) >= FIF_BMP && (fifmt) <= FIF_JXR)

/**
 * 销毁环境。
 * 
 * @warning 需要与abcdk_fi_init配对使用。
*/
void abcdk_fi_uninit();

/**
 * 初始化环境。
 *  
 * @warning 需要与abcdk_fi_uninit配对使用。
*/
void abcdk_fi_init(int load_local_plugins_only);

/**
 * 保存图像。
 * 
 * @param bits 像素位宽，支持24、32位。
 * 
 * @return 0 成功，!0 失败。
*/
int abcdk_fi_save(FREE_IMAGE_FORMAT fifmt, int fiflag, int fd, const uint8_t *data,
                  uint32_t stride, uint32_t width, uint32_t height, uint8_t bits);

/**
 * 保存图像。
 * 
 * @return 0 成功，!0 失败。
*/
int abcdk_fi_save2(FREE_IMAGE_FORMAT fifmt, int fiflag, const char *file, const uint8_t *data,
                   uint32_t stride, uint32_t width, uint32_t height, uint8_t bits);

#endif //FREEIMAGE_H

__END_DECLS

#endif //ABCDKUTIL_FREEIMAGE_H