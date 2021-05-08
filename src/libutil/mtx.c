/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "mtx.h"

int good_mtx_move_medium(int fd, uint16_t t, uint16_t src, uint16_t dst,
                         uint32_t timeout, good_scsi_io_stat *stat)
{

    uint8_t cdb[12] = {0};

    cdb[0] = 0xA5; /* 0xA5 Move Medium code */
    *GOOD_PTR2PTR(uint16_t, cdb, 2) = good_endian_hton16(t);
    *GOOD_PTR2PTR(uint16_t, cdb, 4) = good_endian_hton16(src);
    *GOOD_PTR2PTR(uint16_t, cdb, 6) = good_endian_hton16(dst);

    return good_scsi_sgioctl2(fd, SG_DXFER_NONE, cdb, 12, NULL, 0, timeout, stat);
}

int good_mtx_prevent_medium_removal(int fd, int disable,
                                    uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[6] = {0};

    cdb[0] = 0x1E; /* 0x1E Prevent Allow Medium Removal  */
    cdb[4] = disable ? 0x01 : 0x00;

    return good_scsi_sgioctl2(fd, SG_DXFER_NONE, cdb, 6, NULL, 0, timeout, stat);
}

int good_mtx_mode_sense(int fd, uint8_t pctrl, uint8_t pcode, uint8_t spcode,
                        uint8_t *transfer, uint8_t transferlen,
                        uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[6] = {0};

    cdb[0] = 0x1A; /* 0x1A Mode Sense  */
    cdb[1] = 0x08;
    cdb[2] = (pctrl << 6) | (pcode & 0x3F);
    cdb[3] = spcode;
    cdb[4] = transferlen;

    return good_scsi_sgioctl2(fd, SG_DXFER_FROM_DEV, cdb, 6, transfer, transferlen, timeout, stat);
}

int good_mtx_read_element_status(int fd, uint8_t type, uint16_t address, uint16_t count,
                                 uint8_t *transfer, uint32_t transferlen,
                                 uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[12] = {0};

    cdb[0] = 0xB8;                                                 /* 0xB8 Read Element Status */
    cdb[1] = 0x10 | (type & 0x0F);                                 /*VolTag = 1*/
    *GOOD_PTR2PTR(uint16_t, cdb, 2) = good_endian_hton16(address); /*2,3*/
    *GOOD_PTR2PTR(uint16_t, cdb, 4) = good_endian_hton16(count);   /*4,5*/
    cdb[6] = 0x01;                                                 /*DVCID = 1*/
    good_endian_hton24(cdb + 7, transferlen);                      /*7,8,9*/

    return good_scsi_sgioctl2(fd, SG_DXFER_FROM_DEV, cdb, 12, transfer, transferlen, timeout, stat);
}

void good_mtx_parse_element_status(good_tree_t *father, uint8_t *element, uint16_t count)
{
    assert(father != NULL && element != NULL && count > 0);

    /**/
    uint8_t type = element[8];
    int pvoltag = (element[9] & 0x80) ? 1 : 0;
    int avoltag = (element[9] & 0x40) ? 1 : 0;
    uint16_t psize = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, element, 10));
    uint8_t *ptr = element + 16; /*First Page*/

    for (uint16_t i = 0; i < count; i++)
    {
        /*申请节点。*/
        size_t sizes[5] = {sizeof(uint16_t), sizeof(uint8_t), sizeof(uint8_t), 36, 32};
        good_tree_t *one = good_tree_alloc2(sizes, 5);

        /*如果节点申请失败提结束。*/
        if (one == NULL)
            break;

        /*是否有条码字段。*/
        uint8_t volsize = (pvoltag ? 36 : 0);

        /*获取部分字段。*/
        *GOOD_PTR2PTR(uint16_t, one->alloc->pptrs[GOOD_MTX_ELEMENT_ADDR], 0) = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, ptr, 0));
        *one->alloc->pptrs[GOOD_MTX_ELEMENT_TYPE] = type;
        *one->alloc->pptrs[GOOD_MTX_ELEMENT_ISFULL] = (ptr[2] & 0x01) ? 1 : 0;
        if (volsize > 0)
            memcpy(one->alloc->pptrs[GOOD_MTX_ELEMENT_BARCODE], ptr + 12, volsize);

        /*是否有驱动器信息。*/
        uint8_t dvcid_set = ptr[12 + volsize] & 0x0F;

        /*驱动器才有下面的数据。*/
        if ((GOOD_MXT_ELEMENT_DXFER == type) && (dvcid_set != 0))
        {
            uint8_t dvcid_type = ptr[13 + volsize] & 0x0F;
            uint8_t dvcid_length = ptr[15 + volsize];
            /**/
            if (dvcid_type == 0x00)
            {
                /*Only Serial Number.*/
                memcpy(one->alloc->pptrs[GOOD_MTX_ELEMENT_DVCID], ptr + 16 + volsize, dvcid_length);
            }
            else if (dvcid_type == 0x01)
            {
                if (dvcid_length == 0x0A || dvcid_length == 0x20)
                {
                    /*
                     * Only Serial Number. 
                     *
                     * Is Spectra Tape Libraries?
                    */
                    memcpy(one->alloc->pptrs[GOOD_MTX_ELEMENT_DVCID], ptr + 16 + volsize, dvcid_length);
                }
                else
                {
                    /*
                     * type == 0x01, which is equivalent to the drive's Inquiry page 83h.
                     * 
                     * VENDOR(8)+PRODUCT(16)+SERIAL(10)
                    */
                    memcpy(one->alloc->pptrs[GOOD_MTX_ELEMENT_DVCID], ptr + 16 + volsize + 8 + 16, 10);
                }
            }
            else
            {
                /*0x02~0x0f.*/;
            }
        }

        /*清除两端的空格。*/
        good_strtrim(one->alloc->pptrs[GOOD_MTX_ELEMENT_BARCODE], isblank, 2);
        good_strtrim(one->alloc->pptrs[GOOD_MTX_ELEMENT_DVCID], isblank, 2);

        /*添加到子节点末尾。*/
        good_tree_insert2(father, one, 0);
    }
}