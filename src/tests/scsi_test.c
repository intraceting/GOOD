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

void test_get_sn()
{
    int fd = good_open("/dev/sda",0,0,0);

    uint8_t status;
    
    uint8_t type = 0;
    char vendor[32] = {0};
    char product[64] = {0};

    assert(good_scsi_inquiry_baseinfo(fd,&type,vendor,product,NULL,0,&status,3000)==0);

    printf("type:%s(%hhu),vendor:%s,product:%s\n",good_scsi_type2string(type),type,vendor,product);

    char sn[64]={0};

    assert(good_scsi_inquiry_sn(fd,sn,NULL,0,&status,3000)==0);

    printf("sn:%s\n",sn);


    good_closep(&fd);
}

int main(int argc, char **argv)
{

    test_get_sn();

    return 0;
}

