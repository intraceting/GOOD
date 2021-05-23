/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "mt.h"

const char *good_mt_density2string(uint8_t density)
{
    const char * szptr_result = "Reserved";
    /**/
    switch(density)
    {
        case 0x00:
            return "default";
        case 0x40:
            return "LTO-1";
        case 0x42:
            return "LTO-2";
        case 0x44:
            return "LTO-3";
        case 0x46:
            return "LTO-4";
        case 0x58:
            return "LTO-5";
        case 0x5A:
            return "LTO-6";
        case 0x5C:
            return "LTO-7";
        case 0x5D:
            return "LTO-M8";
        case 0x5E:
            return "LTO-8";
        default:
            return "Reserved";
    }
}

int good_mt_operate(int fd,short cmd,int param)
{
    struct mtop mp = {0};

    assert(fd >=0);

    mp.mt_op = cmd;
    mp.mt_count = param;

    return ioctl(fd,MTIOCTOP,&mp);
}

int good_mt_verify(int fd,uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[6] = {0};
    
    cdb[0] = 0x13; /* 0x13 Verify  */
    cdb[1] |= 0x20; /*the VTE bit is set to one.*/

    return good_scsi_sgioctl2(fd, SG_DXFER_NONE, cdb, 6, NULL,0, timeout, stat);
}

int good_mt_locate(int fd, int cp, uint8_t part, uint64_t block,
                   uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[16] = {0};

    cdb[0] = 0x92; /* 0x92 Locate  */
    cdb[1] |= (cp ? 0x02 : 0x00); /* 0x02 Change Partition */
    cdb[3] = part;
    GOOD_PTR2OBJ(uint64_t, cdb,4) = good_endian_hton64(block); /*4,5,6,7,8,9,10,11*/

    return good_scsi_sgioctl2(fd, SG_DXFER_NONE, cdb, 16, NULL,0, timeout, stat);
}

int good_mt_read_position(int fd, uint64_t *block, uint64_t *file, uint32_t *part,
                          uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[10] = {0};
    uint8_t buf[32] = {0};
    int chk;

    cdb[0] = 0x34; /* 0x34 Read Position   */
    cdb[1] = 0x06; /* 0x06 Long form */

    chk = good_scsi_sgioctl2(fd,SG_DXFER_FROM_DEV,cdb,10,buf, 32, timeout, stat);

    if(chk == 0)
    {
        if(block)
            *block = good_endian_ntoh64(GOOD_PTR2U64(buf,8)); /*8,9,10,11,12,13,14,15*/
        if(file)
            *file = good_endian_ntoh64(GOOD_PTR2U64(buf,16)); /*16,17,18,19,20,21,22,23*/
        if(part)
            *part = good_endian_ntoh32(GOOD_PTR2U32(buf,4)); /*4,5,6,7*/
    }

    return chk;
}

good_allocator_t *good_mt_read_attribute(int fd, uint8_t part, uint16_t id,
                                         uint32_t timeout, good_scsi_io_stat *stat)
{
    good_allocator_t *alloc = NULL;
    uint16_t len = 0;

    uint8_t buf[255]= {0};
    uint8_t cdb[16] = {0};
    int chk;

    cdb[0] = 0x8C; /* 0x8C Read Attribute */
    cdb[1] = 0x00; /* 0x00 VALUE  */
    cdb[7] = part;
    GOOD_PTR2U16(cdb ,8) = good_endian_hton16(id); /*8,9*/
    GOOD_PTR2U32(cdb,10) = good_endian_hton32(sizeof(buf)); /*10,11,12,13*/

    chk = good_scsi_sgioctl2(fd,SG_DXFER_FROM_DEV,cdb,16,buf, sizeof(buf), timeout, stat);
    if(chk!=0)
        return NULL;

    //printf("%u\n",good_endian_ntoh32(GOOD_PTR2U32(buf, 0)));

    len = good_endian_ntoh16(GOOD_PTR2U16(buf, 7)); /*7,8*/

    size_t sizes[5] = {sizeof(uint16_t), sizeof(uint8_t), sizeof(uint8_t), sizeof(uint16_t), len + 1};
    alloc = good_allocator_alloc(sizes,5,0);
    if(!alloc)
        return NULL;

    GOOD_PTR2U16(alloc->pptrs[GOOD_MT_ATTR_ID], 0) = good_endian_ntoh16(GOOD_PTR2U16(buf, 4)); /*4,5*/
    GOOD_PTR2U8(alloc->pptrs[GOOD_MT_ATTR_READONLY], 0) = (buf[6] >> 7);
    GOOD_PTR2U8(alloc->pptrs[GOOD_MT_ATTR_FORMAT], 0) = (buf[6] & 0x03);
    GOOD_PTR2U16(alloc->pptrs[GOOD_MT_ATTR_LENGTH], 0) = good_endian_ntoh16(GOOD_PTR2U16(buf, 7)); /*7,8*/
    memcpy(alloc->pptrs[GOOD_MT_ATTR_VALUE], GOOD_PTR2PTR(void,buf,9), len);

    return alloc;
}

int good_mt_write_attribute(int fd, uint8_t part, const good_allocator_t *attr,
                            uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t buf[255]= {0};
    uint8_t cdb[16] = {0};

    assert(attr != NULL);
    assert(4 + 5 + GOOD_PTR2U16(attr->pptrs[GOOD_MT_ATTR_LENGTH], 0) <= 255);

    cdb[0] = 0x8D; /* 0x8D Write Attribute */
    cdb[1] = 0x01; /* 0x01 Write SYNC */
    cdb[7] = part;
    GOOD_PTR2U32(cdb,10) = good_endian_hton32(sizeof(buf)); /*10,11,12,13*/

    GOOD_PTR2U32(buf, 0) = good_endian_hton32(4 + GOOD_PTR2U16(attr->pptrs[GOOD_MT_ATTR_LENGTH], 0));
    GOOD_PTR2U32(buf, 4) = good_endian_hton16(GOOD_PTR2U16(attr->pptrs[GOOD_MT_ATTR_ID], 0));
    GOOD_PTR2U32(buf, 6) |= (GOOD_PTR2U8(attr->pptrs[GOOD_MT_ATTR_FORMAT], 0) & 0x03);
    GOOD_PTR2U32(buf, 7) = good_endian_hton16(GOOD_PTR2U16(attr->pptrs[GOOD_MT_ATTR_LENGTH], 0));
    memcpy(GOOD_PTR2PTR(void,buf,9),attr->pptrs[GOOD_MT_ATTR_VALUE],GOOD_PTR2U16(attr->pptrs[GOOD_MT_ATTR_LENGTH],0));

    return good_scsi_sgioctl2(fd,SG_DXFER_TO_DEV,cdb,16,buf, sizeof(buf), timeout, stat);
}
