/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_MT_H
#define GOOD_UTIL_MT_H

#include "general.h"
#include "allocator.h"
#include "scsi.h"

/**
 * 属性的字段。
*/
enum _good_mt_attr_field
{
    /** ID 字段索引.*/
    GOOD_MT_ATTR_ID = 0,
#define GOOD_MT_ATTR_ID GOOD_MT_ATTR_ID

    /** 只读 字段索引.*/
    GOOD_MT_ATTR_READONLY = 1,
#define GOOD_MT_ATTR_READONLY GOOD_MT_ATTR_READONLY

    /** 格式 字段索引. */
    GOOD_MT_ATTR_FORMAT = 2,
#define GOOD_MT_ATTR_FORMAT GOOD_MT_ATTR_FORMAT

    /** 长度 字段索引. */
    GOOD_MT_ATTR_LENGTH = 3,
#define GOOD_MT_ATTR_LENGTH GOOD_MT_ATTR_LENGTH

    /** 数据 字段索引. */
    GOOD_MT_ATTR_DATA = 4
#define GOOD_MT_ATTR_DATA GOOD_MT_ATTR_DATA

};

/**
 * 磁带型号数字编码转字符串编码。
 * 
 * 参考mt命令。
 * 
 * @return 字符串的指针。
 */
const char *good_mt_density2string(uint8_t density);

/**
 * 磁带操作。
 * 
 * @return 0 成功，-1 失败。
*/
int good_mt_operate(int fd,short cmd,int param);

/**压缩功能开关。*/
#define good_mt_compression(fd, enable)     good_mt_operate(fd, MTCOMPRESSION, enable)

/**设置读写块大小。*/
#define good_mt_blocksize(fd, blocks)       good_mt_operate(fd, MTSETBLK, blocks)

/**写FILEMARK标记。*/
#define good_mt_writefm(fd, counts)         good_mt_operate(fd, MTWEOF, counts)

/**倒带。*/
#define good_mt_rewind(fd, offline)         good_mt_operate(fd, offline ? MTOFFL : MTREW, 0)

/**加载磁带。*/
#define good_mt_load(fd)                    good_mt_operate(fd, MTLOAD, 0)

/**卸载磁带*/
#define good_mt_unload(fd)                  good_mt_operate(fd, MTUNLOAD, 0)

/**禁止卸载磁带。*/
#define good_mt_lock(fd)                    good_mt_operate(fd, MTLOCK, 0)

/**允许卸载磁带。*/
#define good_mt_unlock(fd)                  good_mt_operate(fd, MTUNLOCK, 0)

/**擦除磁带数据。*/
#define good_mt_erase(fd, physical)         good_mt_operate(fd, MTERASE, physical) /*dangerous*/

/**改变活动分区。*/
#define good_mt_part(part)                  good_mt_operate(fd, MTSETPART, part);

/**
 * 磁带较验。
 * 
 * cdb = 0x13
 * 
 * @return 0 成功，-1 失败。
*/
int good_mt_verify(int fd,uint32_t timeout, good_scsi_io_stat *stat);

/**
 * 定位磁头的位置。
 * 
 * cdb = 0x92
 * 
 * @param cp 是否改变当前活动分区。
 * @param part 分区号。
 * @param block 逻辑块索引。
 * 
 * @return 0 成功，-1 失败。
 * 
 * @warning  New tape(KEY = 0x08,ASC = 0x14,ASCQ = 0x03).
 * @warning  End of data (KEY = 0x08,ASC = 0x00,ASCQ = 0x05).
*/
int good_mt_locate(int fd, int cp, uint8_t part, uint64_t block,
                   uint32_t timeout, good_scsi_io_stat *stat);


/**
 * 读取磁头当前位置。
 * 
 * cdb = 0x34
 * 
 * @param block 返回逻辑块索引，为NULL(0)忽略。
 * @param file 返回逻辑文件的索引，为NULL(0)忽略。
 * @param part 返回分区号，为NULL(0)忽略。
 * 
 * @return 0 成功，-1 失败。
*/
int good_mt_read_position(int fd, uint64_t *block, uint64_t *file, uint32_t *part,
                          uint32_t timeout, good_scsi_io_stat *stat);


/** 
 * 读取磁带属性。
 * 
 * cdb = 0x8C
 * 
 * @return 0 成功，-1 失败。
*/
int good_mt_read_attribute(int fd, uint8_t part, uint16_t fid, uint8_t *transfer, uint32_t transferlen,
                           uint32_t timeout, good_scsi_io_stat *stat);

/**
 * 分析属性内容。
 * 
 * 如果是二进制数据，则数据字节序为网络字节序。
 * 
 * @see good_mt_read_attribute
 * @see good_mt_write_attribute
*/
good_allocator_t *good_mt_parse_attribute(const uint8_t *attribute);

#endif //GOOD_UTIL_MT_H