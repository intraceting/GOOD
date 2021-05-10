/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "mtx.h"

int good_mtx_inventory(int fd, uint16_t address, uint16_t count,
                       uint32_t timeout, good_scsi_io_stat *stat)
{

    uint8_t cdb[10] = {0};

    cdb[0] = 0x37; /* 0x37 or E7 Initialize Element Status With Range */
    GOOD_PTR2OBJ(uint8_t, cdb, 1) |= (count > 0 ? 0x01 : 0);
    GOOD_PTR2OBJ(uint16_t, cdb, 2) = good_endian_hton16(address);
    GOOD_PTR2OBJ(uint16_t, cdb, 6) = good_endian_hton16(count);

    return good_scsi_sgioctl2(fd, SG_DXFER_NONE, cdb, 10, NULL, 0, timeout, stat);
}                       

int good_mtx_move_medium(int fd, uint16_t t, uint16_t src, uint16_t dst,
                         uint32_t timeout, good_scsi_io_stat *stat)
{

    uint8_t cdb[12] = {0};

    cdb[0] = 0xA5; /* 0xA5 Move Medium code */
    GOOD_PTR2OBJ(uint16_t, cdb, 2) = good_endian_hton16(t);
    GOOD_PTR2OBJ(uint16_t, cdb, 4) = good_endian_hton16(src);
    GOOD_PTR2OBJ(uint16_t, cdb, 6) = good_endian_hton16(dst);

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
    GOOD_PTR2OBJ(uint16_t, cdb, 2) = good_endian_hton16(address); /*2,3*/
    GOOD_PTR2OBJ(uint16_t, cdb, 4) = good_endian_hton16(count);   /*4,5*/
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
    uint16_t psize = good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, element, 10));
    uint8_t *ptr = element + 16; /*First Page*/

    for (uint16_t i = 0; i < count; i++)
    {
        /*申请节点。*/
        size_t sizes[5] = {sizeof(uint16_t), sizeof(uint8_t), sizeof(uint8_t), 36+1, 32+1};
        good_tree_t *one = good_tree_alloc2(sizes, 5);

        /*如果节点申请失败提结束。*/
        if (one == NULL)
            break;

        /*是否有条码字段。*/
        uint8_t volsize = (pvoltag ? 36 : 0);

        /*获取部分字段。*/
        GOOD_PTR2OBJ(uint16_t, one->alloc->pptrs[GOOD_MTX_ELEMENT_ADDR], 0) = good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, ptr, 0));
        GOOD_PTR2OBJ(uint8_t, one->alloc->pptrs[GOOD_MTX_ELEMENT_TYPE],0) = type;
        GOOD_PTR2OBJ(uint8_t, one->alloc->pptrs[GOOD_MTX_ELEMENT_ISFULL],0) = ((ptr[2] & 0x01) ? 1 : 0);
        if (volsize > 0)
            memcpy(one->alloc->pptrs[GOOD_MTX_ELEMENT_BARCODE], ptr + 12, volsize);

        /*是否有机械臂或驱动器信息。*/
        uint8_t dvcid_set = ptr[12 + volsize] & 0x0F;

        /*0x01或0x02有效。*/
        if(dvcid_set == 0)
            goto next;

        /*机械臂或驱动器才有下面的数据。*/
        if (GOOD_MXT_ELEMENT_CHANGER == type ||GOOD_MXT_ELEMENT_DXFER == type)
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

next:

        /*清除两端的空格。*/
        good_strtrim(one->alloc->pptrs[GOOD_MTX_ELEMENT_BARCODE], isspace, 2);
        good_strtrim(one->alloc->pptrs[GOOD_MTX_ELEMENT_DVCID], isspace, 2);

        /*添加到子节点末尾。*/
        good_tree_insert2(father, one, 0);

        /*下一页。*/
        ptr += psize;
    }
}

int good_mtx_inquiry_element_status(good_tree_t *father,int fd,uint32_t timeout, good_scsi_io_stat *stat)
{
    char buf[255] = {0};
    int buf2size = 0;
    uint8_t *buf2 = NULL;
    int chk;

    chk = good_mtx_mode_sense(fd, 0, 0x1d, 0, buf, 255, timeout, stat);
    if(chk != 0)
        return -1;

    /**/
    buf2size = 0x00ffffff;/*15MB enough!*/
    buf2 = (uint8_t*) good_heap_alloc(buf2size);
    if(!buf2)
        return -1;

    /*GOOD_MXT_ELEMENT_CHANGER:4+2,4+4*/
    chk = good_mtx_read_element_status(fd, GOOD_MXT_ELEMENT_CHANGER,
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 2)),
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 4)),
                                       buf2, 2 * 1024 * 1024, -1, stat);
    if(chk != 0)
        goto final;

    good_mtx_parse_element_status(father,buf2,good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 4)));

    /*GOOD_MXT_ELEMENT_STORAGE:4+6,4+8*/
    chk = good_mtx_read_element_status(fd, GOOD_MXT_ELEMENT_STORAGE,
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 6)),
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 8)),
                                       buf2, 2 * 1024 * 1024, -1, stat);
    if(chk != 0)
        goto final;

    good_mtx_parse_element_status(father,buf2,good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 8)));

    /*GOOD_MXT_ELEMENT_IE_PORT:4+10,4+12*/
    chk = good_mtx_read_element_status(fd, GOOD_MXT_ELEMENT_IE_PORT,
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 10)),
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 12)),
                                       buf2, 2 * 1024 * 1024, -1, stat);
    if(chk != 0)
        goto final;

    good_mtx_parse_element_status(father,buf2,good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 12)));

    /*GOOD_MXT_ELEMENT_DXFER:4+14,4+16*/
    chk = good_mtx_read_element_status(fd, GOOD_MXT_ELEMENT_DXFER,
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 14)),
                                       good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 16)),
                                       buf2, 2 * 1024 * 1024, -1, stat);
    if(chk != 0)
        goto final;

    good_mtx_parse_element_status(father,buf2,good_endian_ntoh16(GOOD_PTR2OBJ(uint16_t, buf, 4 + 16)));

final:

    good_heap_freep((void**)&buf2);
    
    return chk;
}