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
    int fd = good_open("/dev/sg10",0,0,0);

    good_scsi_io_stat stat = {0};
    
    uint8_t type = 0;
    char vendor[32] = {0};
    char product[64] = {0};
    char sn[64]={0};

    assert(good_scsi_inquiry_standard(fd,&type,vendor,product,3000,&stat)==0);

    printf("type:%s(%hhu),vendor:%s,product:%s",good_scsi_type2string(type),type,vendor,product);

    assert(good_scsi_inquiry_serial(fd,NULL,sn,3000,&stat)==0);
    printf(",sn:%s\n",sn);

    good_closep(&fd);
}

int dump2(size_t deep, good_tree_t *node, void *opaque)
{
    if(deep==0)
    {
        good_tree_fprintf(stderr,deep,node,"haha\n");
    }
    else
    {
        good_tree_fprintf(stderr,deep,node,"%-10hu\t|%-10hhu\t|%-10hhu\t|%-36s\t|%-20s\t|\n",
        GOOD_PTR2OBJ(uint16_t, node->alloc->pptrs[GOOD_MTX_ELEMENT_ADDR], 0),
        GOOD_PTR2OBJ(uint8_t,node->alloc->pptrs[GOOD_MTX_ELEMENT_TYPE],0),
        GOOD_PTR2OBJ(uint8_t,node->alloc->pptrs[GOOD_MTX_ELEMENT_ISFULL],0),
        node->alloc->pptrs[GOOD_MTX_ELEMENT_BARCODE],
        node->alloc->pptrs[GOOD_MTX_ELEMENT_DVCID]);
    }

    return 1;
}

void traversal(good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_iterator_t it = {0,dump2,NULL};
    good_tree_scan(root,&it);

    printf("\n-------------------------------------\n");
}


void test_move()
{
    int fd = good_open("/dev/sg10",0,0,0);

    

    good_scsi_io_stat stat = {0};

    assert(good_mtx_inventory(fd,0,0,1000,&stat)==0);

    assert(good_mtx_inventory(fd,5000,10,1000,&stat)==0);

    for (int i = 0; i < 4; i++)
    {
        assert(good_mtx_move_medium(fd, 0, 500+i, 1000+i, -1, &stat) == 0);

        printf("%hhx,%hhx,%hhx\n", good_scsi_sense_key(stat.sense), good_scsi_sense_code(stat.sense), good_scsi_sense_qualifier(stat.sense));
    }

    // assert(good_mtx_prevent_medium_removal(fd,1,-1,&stat)==0);

    //  printf("%hhx,%hhx,%hhx\n",good_scsi_sense_key(stat.sense),good_scsi_sense_code(stat.sense),good_scsi_sense_qualifier(stat.sense));

    // assert(good_mtx_prevent_medium_removal(fd,0,-1,&stat)==0);

    // printf("%hhx,%hhx,%hhx\n",good_scsi_sense_key(stat.sense),good_scsi_sense_code(stat.sense),good_scsi_sense_qualifier(stat.sense));

    // char buf[255] = {0};
    // assert(good_mtx_mode_sense(fd, 0, 0x1d, 0, buf, 255, -1, &stat) == 0);

    // /**/
    // uint16_t changer_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 2));
    // uint16_t changer_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf,4 + 4));
    // uint16_t storage_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 6));
    // uint16_t storage_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 8));
    // uint16_t storage_ie_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 10));
    // uint16_t storage_ie_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 12));
    // uint16_t driver_address = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 14));
    // uint16_t driver_count = good_endian_ntoh16(*GOOD_PTR2PTR(uint16_t, buf, 4 + 16));

    // /**/
    // int buf2size = (0x00ffffff); /*15MB enough!*/
    // uint8_t * buf2 = (uint8_t*) good_heap_alloc(buf2size);

    // assert(good_mtx_read_element_status(fd,GOOD_MXT_ELEMENT_DXFER,driver_address,driver_count,buf2,2*1024*1024,-1,&stat)==0);

    good_tree_t *t = good_tree_alloc(NULL);

    assert(good_mtx_inquiry_element_status(t,fd,-1,&stat)==0);

    traversal(t);

    good_tree_free(&t);

    good_closep(&fd);
}

int main(int argc, char **argv)
{

    test_get_sn();

    test_move();

    return 0;
}

