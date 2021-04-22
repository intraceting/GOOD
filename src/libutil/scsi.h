/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_SCSI_H
#define GOOD_UTIL_SCSI_H

#include "general.h"

/**
 * 通用的SCSI-V3指令接口。
 * 
 * @param hdr 指令的指针。 in sg.h
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_ioctl(int fd,struct sg_io_hdr *hdr);

/**
 * 通用的SCSI-V3指令接口。
 * 
 * @param timeout 超时(毫秒)。UINT_MAX 直到完成或出错。
 *
 * @return 0 成功，-1 失败。
*/
int good_scsi_ioctl2(int fd,
                     int direction,
                     uint8_t *cdb, uint8_t cdblen,
                     uint8_t *transfer, unsigned int transferlen,
                     uint8_t *sense, uint8_t senselen,
                     uint8_t *status,
                     unsigned int timeout);

/** 
 * 测试设备是否准备好。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_test(int fd,uint8_t *sense, uint8_t senselen,uint8_t *status,unsigned int timeout);

/** 
 * 测试设备是否准备好。
 * 
 * 0x00H
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_test(int fd,uint8_t *sense, uint8_t senselen,uint8_t *status,unsigned int timeout);

/** 
 * 查询设备信息。
 * 
 * 0x12H
 * 
 * @param vpd 是否查询虚拟页面。0 否，!0 是。
 * @param vid 虚拟页面ID。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_inquiry(int fd,
                      int vpd, uint8_t vid,
                      uint8_t *transfer, unsigned int transferlen,
                      uint8_t *sense, uint8_t senselen,
                      uint8_t *status,
                      unsigned int timeout);

/** 
 * 获取设备序列号。
 * 
 * cdb = 0x12H, vpd = 1, vid = 0x80
 * 
 * @param status 状态的指针，不能为NULL(0)。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_inquiry_sn(int fd,
                         char buf[64],
                         uint8_t *sense, uint8_t senselen,
                         uint8_t *status,
                         unsigned int timeout);

/** 
 * 获取设备基本信息。
 * 
 * cdb = 0x12H, vpd = 0, vid = 0x00
 * 
 * @param status 状态的指针，不能为NULL(0)。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_inquiry_baseinfo(int fd,
                               uint8_t *type,
                               char vendor[16],
                               char product[32],
                               uint8_t *sense, uint8_t senselen,
                               uint8_t *status,
                               unsigned int timeout);

/**
 * 设备类型数字编码转字符串编码。
 * 
 * 参考lsscsi命令。
 * 
 * @return 字符串的指针。
 */
const char* good_scsi_type2string(uint8_t type);

#endif //GOOD_UTIL_SCSI_H