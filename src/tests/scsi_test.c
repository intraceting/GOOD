/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "libutil/general.h"
#include "libutil/scsi.h"
#include "libutil/mtx.h"

void test_get_sn()
{
    int fd = good_open("/dev/sg7",0,0,0);

    good_scsi_io_stat stat = {0};
    
    uint8_t type = 0;
    char vendor[32] = {0};
    char product[64] = {0};

    assert(good_scsi_inquiry_baseinfo(fd,&type,vendor,product,3000,&stat)==0);

    printf("type:%s(%hhu),vendor:%s,product:%s\n",good_scsi_type2string(type),type,vendor,product);

    char sn[64]={0};

    assert(good_scsi_inquiry_sn(fd,sn,3000,&stat)==0);

    printf("sn:%s\n",sn);


    good_closep(&fd);
}

void test_move()
{
    int fd = good_open("/dev/sg9",0,0,0);

    good_scsi_io_stat stat = {0};

     assert(good_mtx_move_medium(fd,1,1000,500,-1,&stat)==0);

      printf("%hhx,%hhx,%hhx\n",good_scsi_sense_key(stat.sense),good_scsi_sense_code(stat.sense),good_scsi_sense_qualifier(stat.sense));

    // assert(good_mtx_prevent_medium_removal(fd,1,-1,&stat)==0);

    //  printf("%hhx,%hhx,%hhx\n",good_scsi_sense_key(stat.sense),good_scsi_sense_code(stat.sense),good_scsi_sense_qualifier(stat.sense));

    // assert(good_mtx_prevent_medium_removal(fd,0,-1,&stat)==0);

    // printf("%hhx,%hhx,%hhx\n",good_scsi_sense_key(stat.sense),good_scsi_sense_code(stat.sense),good_scsi_sense_qualifier(stat.sense));

    char buf[255] = {0};
    assert(good_mtx_mode_sense(fd, 0, 0x1d, 0, buf, 255, -1, &stat) == 0);

    /**/
    uint16_t changer_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 2));
    uint16_t changer_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf,4 + 4));
    uint16_t storage_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 6));
    uint16_t storage_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 8));
    uint16_t storage_ie_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 10));
    uint16_t storage_ie_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 12));
    uint16_t driver_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 14));
    uint16_t driver_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 16));

    /**/
    int buf2size = (0x00ffffff); /*15MB enough!*/
    uint8_t * buf2 = (uint8_t*) good_heap_alloc(buf2size);

    assert(good_mtx_read_element_status(fd,GOOD_MXT_ELEMENT_DXFER,driver_address,driver_count,buf2,2*1024*1024,-1,&stat)==0);

    good_closep(&fd);
}

int main(int argc, char **argv)
{

  //  test_get_sn();

    test_move();

    return 0;
}

