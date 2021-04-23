/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "scsi.h"

int good_scsi_sgioctl(int fd,struct sg_io_hdr *hdr)
{
    assert(fd >= 0 && hdr != NULL);

    assert(hdr->interface_id == 'S');
    assert(hdr->dxfer_direction == SG_DXFER_NONE || hdr->dxfer_direction == SG_DXFER_TO_DEV || 
            hdr->dxfer_direction == SG_DXFER_FROM_DEV || hdr->dxfer_direction == SG_DXFER_TO_FROM_DEV);
    assert(hdr->cmdp != NULL && hdr->cmd_len > 0);
    assert((hdr->dxferp != NULL && hdr->dxfer_len > 0) ||(hdr->dxferp == NULL && hdr->dxfer_len <= 0));
    assert((hdr->sbp != NULL && hdr->mx_sb_len > 0) ||(hdr->sbp == NULL && hdr->mx_sb_len <= 0));

    return ioctl(fd,SG_IO,hdr);
}

int good_scsi_sgioctl2(int fd,
                       int direction,
                       uint8_t *cdb, uint8_t cdblen,
                       uint8_t *transfer, unsigned int transferlen,
                       uint8_t *sense, uint8_t senselen,
                       uint8_t *status,
                       unsigned int timeout)
{
    struct sg_io_hdr hdr = {0};
    int chk;

    assert(fd >= 0 && cdb != NULL && cdblen > 0);
    assert((transfer != NULL && transferlen > 0) ||(transfer == NULL && transferlen <= 0));
    assert((sense != NULL && senselen > 0) ||(sense == NULL && senselen <= 0));

    hdr.interface_id = 'S';
    hdr.dxfer_direction = direction;
    hdr.cmdp = cdb;
    hdr.cmd_len = cdblen;
    hdr.dxferp = transfer;
    hdr.dxfer_len = transferlen;
    hdr.sbp = sense;
    hdr.mx_sb_len = senselen;
    hdr.timeout = timeout;

    chk = good_scsi_sgioctl(fd,&hdr);
    if(chk != 0)
        return -1;

    if(status)
        *status = hdr.status;

    return 0;
}

int good_scsi_test(int fd,uint8_t *sense, uint8_t senselen,uint8_t *status,unsigned int timeout)
{
    uint8_t cdb[6] = {0};
    
    cdb[0] = 0x00; /*00H is TEST UNIT READY*/

    return good_scsi_sgioctl2(fd,SG_DXFER_NONE,cdb,6,NULL,0,sense,senselen,status,timeout);
}

int good_scsi_inquiry(int fd,
                      int vpd, uint8_t vid,
                      uint8_t *transfer, unsigned int transferlen,
                      uint8_t *sense, uint8_t senselen,
                      uint8_t *status,
                      unsigned int timeout)
{
    uint8_t cdb[6] = {0};
  
    cdb[0] = 0x12; /*12H is INQUIRY*/
    cdb[1] = (vpd ? 0x01 : 0x00); /* Enable Vital Product Data */
    cdb[2] = (vpd ? vid : 0x00); /* Return PAGE CODE */
    cdb[4] = transferlen;

    return good_scsi_sgioctl2(fd,SG_DXFER_FROM_DEV,cdb,6,transfer,transferlen,sense,senselen,status,timeout);
}

int good_scsi_inquiry_sn(int fd,
                         char buf[64],
                         uint8_t *sense, uint8_t senselen,
                         uint8_t *status,
                         unsigned int timeout)
{
    uint8_t tmp[255] = {0};
    int chk;

    assert(buf != NULL && status != NULL);

    chk = good_scsi_inquiry(fd,1,0x80,tmp,255,sense,senselen,status,timeout);
    if(chk != 0)
        return -1;

    if(*status != GOOD)
        return -1;

    /*
     * Just copy SN。
    */
    memcpy(buf, tmp + 4, GOOD_MIN(tmp[3], 32));

    /*
     * 去掉两端的空格。
    */
    good_strtrim(buf,isspace,2);

    return 0;
}

int good_scsi_inquiry_baseinfo(int fd,
                               uint8_t *type,
                               char vendor[16],
                               char product[32],
                               uint8_t *sense, uint8_t senselen,
                               uint8_t *status,
                               unsigned int timeout)
{
    uint8_t tmp[255] = {0};
    int chk;

    assert(type != NULL || vendor != NULL || product != NULL);
    assert(status != NULL);

    chk = good_scsi_inquiry(fd,0,0x00,tmp,255,sense,senselen,status,timeout);
    if(chk != 0)
        return -1;

    if(*status != GOOD)
        return -1;

    /*
     * Copy TYPE，VENDOR，PRODUCT。
    */
    if(type)
        *type = tmp[0] & 0x1f;
    if(vendor)
        memcpy(vendor, tmp + 8, 8);
    if(product)
        memcpy(product, tmp + 16, 16);

    /*
     * 去掉两端的空格。
    */
    if(vendor)
        good_strtrim(vendor,isspace,2);
    if(product)
        good_strtrim(product,isspace,2);

    return 0;
}

const char* good_scsi_type2string(uint8_t type)
{
    switch(type)
    {
        case TYPE_DISK:
            return "Direct-Access";
        case TYPE_TAPE:
            return "Sequential-Access";
        case 0x02:
            return "Printer";
        case TYPE_PROCESSOR:
            return "Processor";
        case TYPE_WORM:
            return "Write-once";
        case TYPE_ROM:
            return "CD-ROM";
        case TYPE_SCANNER:
            return "Scanner";
        case TYPE_MEDIUM_CHANGER:
            return "Medium Changer";
        case TYPE_ENCLOSURE:
            return "Enclosure";
        default:
            return "Reserved";
    }
}