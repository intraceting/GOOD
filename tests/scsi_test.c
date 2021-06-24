/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "abtkutil/general.h"
#include "abtkutil/scsi.h"
#include "abtkutil/mtx.h"
#include "abtkutil/mt.h"

void test_get_sn()
{
    int fd = abtk_open("/dev/st0",0,0,0);

    abtk_scsi_io_stat stat = {0};
    
    uint8_t type = 0;
    char vendor[32] = {0};
    char product[64] = {0};
    char sn[64]={0};

    assert(abtk_scsi_inquiry_standard(fd,&type,vendor,product,3000,&stat)==0);

    printf("type:%s(%hhu),vendor:%s,product:%s",abtk_scsi_type2string(type),type,vendor,product);

    assert(abtk_scsi_inquiry_serial(fd,NULL,sn,3000,&stat)==0);
    printf(",sn:%s\n",sn);

    abtk_closep(&fd);
}

int dump2(size_t deep, abtk_tree_t *node, void *opaque)
{
    if(deep==0)
    {
        abtk_tree_fprintf(stderr,deep,node,"haha\n");
    }
    else
    {
        abtk_tree_fprintf(stderr, deep, node, "%-6hu\t|%-2hhu\t|%-2hhu\t|%-10s\t|%-10s\t|\n",
                          ABTK_PTR2OBJ(uint16_t, node->alloc->pptrs[ABTK_MTX_ELEMENT_ADDR], 0),
                          ABTK_PTR2OBJ(uint8_t, node->alloc->pptrs[ABTK_MTX_ELEMENT_TYPE], 0),
                          ABTK_PTR2OBJ(uint8_t, node->alloc->pptrs[ABTK_MTX_ELEMENT_ISFULL], 0),
                          node->alloc->pptrs[ABTK_MTX_ELEMENT_BARCODE],
                          node->alloc->pptrs[ABTK_MTX_ELEMENT_DVCID]);
    }

    return 1;
}

void traversal(abtk_tree_t *root)
{
    printf("\n-------------------------------------\n");

    abtk_tree_iterator_t it = {0,dump2,NULL};
    abtk_tree_scan(root,&it);

    printf("\n-------------------------------------\n");
}


void test_mtx()
{
    int fd = abtk_open("/dev/sg9",0,0,0);

    

    abtk_scsi_io_stat stat = {0};

    assert(abtk_mtx_inventory(fd,0,0,1000,&stat)==0);

    assert(abtk_mtx_inventory(fd,5000,10,1000,&stat)==0);

    for (int i = 0; i < 4; i++)
    {
        assert(abtk_mtx_move_medium(fd, 0,1029+i, 500+i, -1, &stat) == 0);

        printf("%hhx,%hhx,%hhx\n", abtk_scsi_sense_key(stat.sense), abtk_scsi_sense_code(stat.sense), abtk_scsi_sense_qualifier(stat.sense));
    }

    // assert(abtk_mtx_prevent_medium_removal(fd,1,-1,&stat)==0);

    //  printf("%hhx,%hhx,%hhx\n",abtk_scsi_sense_key(stat.sense),abtk_scsi_sense_code(stat.sense),abtk_scsi_sense_qualifier(stat.sense));

    // assert(abtk_mtx_prevent_medium_removal(fd,0,-1,&stat)==0);

    // printf("%hhx,%hhx,%hhx\n",abtk_scsi_sense_key(stat.sense),abtk_scsi_sense_code(stat.sense),abtk_scsi_sense_qualifier(stat.sense));

    // char buf[255] = {0};
    // assert(abtk_mtx_mode_sense(fd, 0, 0x1d, 0, buf, 255, -1, &stat) == 0);

    // /**/
    // uint16_t changer_address = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf, 4 + 2));
    // uint16_t changer_count = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf,4 + 4));
    // uint16_t storage_address = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf, 4 + 6));
    // uint16_t storage_count = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf, 4 + 8));
    // uint16_t storage_ie_address = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf, 4 + 10));
    // uint16_t storage_ie_count = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf, 4 + 12));
    // uint16_t driver_address = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf, 4 + 14));
    // uint16_t driver_count = abtk_endian_ntoh16(*ABTK_PTR2PTR(uint16_t, buf, 4 + 16));

    // /**/
    // int buf2size = (0x00ffffff); /*15MB enough!*/
    // uint8_t * buf2 = (uint8_t*) abtk_heap_alloc(buf2size);

    // assert(abtk_mtx_read_element_status(fd,ABTK_MXT_ELEMENT_DXFER,driver_address,driver_count,buf2,2*1024*1024,-1,&stat)==0);

    abtk_tree_t *t = abtk_tree_alloc(NULL);

    assert(abtk_mtx_inquiry_element_status(t,fd,-1,&stat)==0);

    traversal(t);

    abtk_tree_free(&t);

    abtk_closep(&fd);
}

void test_mt()
{
    int fd = abtk_open("/dev/st1",1,0,0);

    abtk_scsi_io_stat stat = {0};

    abtk_mt_compression(fd,0);
    abtk_mt_blocksize(fd,0);

    assert(abtk_mt_verify(fd,3000,&stat)==0);

    assert(abtk_mt_locate(fd,0,0,100,3000,&stat)==0);

     printf("%hhx,%hhx,%hhx\n", abtk_scsi_sense_key(stat.sense), abtk_scsi_sense_code(stat.sense), abtk_scsi_sense_qualifier(stat.sense));


    abtk_write(fd,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",30);
    abtk_write(fd,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",30);

  //  abtk_mt_writefm(fd,10);

    uint64_t block = -1;
    uint64_t file = -1;
    uint32_t part = -1;

     assert(abtk_mt_read_position(fd,&block,&file,&part,3000,&stat)==0);

     printf("%lu,%lu,%u\n",block,file,part);

     abtk_allocator_t *a = abtk_mt_read_attribute(fd,0,0x0000,100,&stat);
     abtk_allocator_t *b = abtk_mt_read_attribute(fd,0,0x0001,100,&stat);
     abtk_allocator_t *c = abtk_mt_read_attribute(fd,0,0x0400,100,&stat);
     abtk_allocator_t *d = abtk_mt_read_attribute(fd,0,0x0401,100,&stat);
     abtk_allocator_t *e = abtk_mt_read_attribute(fd,0,0x0405,100,&stat);
     abtk_allocator_t *f = abtk_mt_read_attribute(fd,0,0x0806,100,&stat);

     abtk_endian_ntoh(a->pptrs[ABTK_MT_ATTR_VALUE],ABTK_PTR2U16(a->pptrs[ABTK_MT_ATTR_LENGTH],0));
     abtk_endian_ntoh(b->pptrs[ABTK_MT_ATTR_VALUE],ABTK_PTR2U16(b->pptrs[ABTK_MT_ATTR_LENGTH],0));

     printf("REMAINING CAPACITY:%lu\n",ABTK_PTR2U64(a->pptrs[ABTK_MT_ATTR_VALUE], 0));
     printf("MAXIMUM CAPACITY:%lu\n",ABTK_PTR2U64(b->pptrs[ABTK_MT_ATTR_VALUE], 0));
     printf("MANUFACTURER:%s\n",c->pptrs[ABTK_MT_ATTR_VALUE]);
     printf("SERIAL NUMBER:%s\n",d->pptrs[ABTK_MT_ATTR_VALUE]);
     printf("DENSITY:%s\n",abtk_mt_density2string(ABTK_PTR2U8(e->pptrs[ABTK_MT_ATTR_VALUE], 0)));
     printf("BARCODE:%s\n",f->pptrs[ABTK_MT_ATTR_VALUE]);


    size_t sizes[5] = {sizeof(uint16_t), sizeof(uint8_t), sizeof(uint8_t), sizeof(uint16_t), 32+1};
    abtk_allocator_t *g = abtk_allocator_alloc(sizes,5,0);

    ABTK_PTR2U16(g->pptrs[ABTK_MT_ATTR_ID],0) = 0x0806;
    ABTK_PTR2U16(g->pptrs[ABTK_MT_ATTR_FORMAT],0) = 1;
    ABTK_PTR2U16(g->pptrs[ABTK_MT_ATTR_LENGTH],0) = 32;

    memcpy(g->pptrs[ABTK_MT_ATTR_VALUE],"aaaaaa",5);

    assert(abtk_mt_write_attribute(fd,0,g,3000,&stat)==0);


     abtk_allocator_unref(&a);
     abtk_allocator_unref(&b);
     abtk_allocator_unref(&c);
     abtk_allocator_unref(&d);
     abtk_allocator_unref(&e);
     abtk_allocator_unref(&f);
     abtk_allocator_unref(&g);

    abtk_closep(&fd);
}

int main(int argc, char **argv)
{

    test_get_sn();

    test_mtx();

    test_mt();

    return 0;
}

