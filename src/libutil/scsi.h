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
 * SCSI接口IO状态信息。
*/
typedef struct _good_scsi_io_stat
{
    /** 
     * 数据的未传输长度。
    */
    int32_t resid;

    /** 
     * 响应信息。 
    */
    uint8_t sense[255];

    /** 
     * 响应信息的写入长度。
    */
    uint8_t senselen_wr;

    /** 
     * 状态。
    */
    uint8_t status;

    /** 
     * 主机状态。
    */
    uint16_t host_status;

    /** 
     * 设备状态。
    */
    uint16_t driver_status;

} good_scsi_io_stat;


/**
 * 通用的SCSI-V3指令接口。
 * 
 * @param hdr 指令的指针。 in sg.h
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_sgioctl(int fd, struct sg_io_hdr *hdr);

/**
 * 通用的SCSI-V3指令接口。
 *
 * @param timeout 超时(毫秒)。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_sgioctl2(int fd, int direction,
                       uint8_t *cdb, uint8_t cdblen,
                       uint8_t *transfer, uint32_t transferlen,
                       uint32_t timeout, good_scsi_io_stat *stat);

/**
 * 获取sense信息KEY字段值。
*/
uint8_t good_scsi_sense_key(uint8_t *sense);

/**
 * 获取sense信息ASC字段值。
*/
uint8_t good_scsi_sense_code(uint8_t *sense);

/**
 * 获取sense信息ASCQ字段值。
*/
uint8_t good_scsi_sense_qualifier(uint8_t *sense);

/** 
 * 测试设备是否准备好。
 * 
 * 0x00H
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_test(int fd, uint32_t timeout, good_scsi_io_stat *stat);

/**
 * 查询状态信息。
 * 
 * cdb = 0x03
 *
 * @return 0 成功，-1 失败。 
*/
int good_mtx_request_sense(int fd,uint32_t timeout, good_scsi_io_stat *stat);

/** 
 * 查询设备信息。
 * 
 * cdb = 0x12H
 * 
 * @param vpd 是否查询虚拟页面。0 否，!0 是。
 * @param vid 虚拟页面ID。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_inquiry(int fd,int vpd, uint8_t vid,
                      uint8_t *transfer, uint32_t transferlen,
                      uint32_t timeout,good_scsi_io_stat *stat);

/** 
 * 获取设备序列号。
 * 
 * cdb = 0x12, vpd = 1, vid = 0x80
 * 
 * @param status 状态的指针，不能为NULL(0)。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_inquiry_sn(int fd,char buf[64],
                         uint32_t timeout,good_scsi_io_stat *stat);

/** 
 * 获取设备基本信息。
 * 
 * cdb = 0x12, vpd = 0, vid = 0x00
 * 
 * @param status 状态的指针，不能为NULL(0)。
 * 
 * @return 0 成功，-1 失败。
*/
int good_scsi_inquiry_baseinfo(int fd,uint8_t *type,char vendor[16],char product[32],
                               uint32_t timeout,good_scsi_io_stat *stat);

/**
 * 设备类型数字编码转字符串编码。
 * 
 * 参考lsscsi命令。
 * 
 * @return 字符串的指针。
 */
const char *good_scsi_type2string(uint8_t type);



#endif //GOOD_UTIL_SCSI_H