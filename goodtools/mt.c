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
#include "goodutil/general.h"
#include "goodutil/getargs.h"
#include "goodutil/scsi.h"
#include "goodutil/mt.h"

/**/
enum _goodmt_cmd
{
    /** 查询驱动器信息。*/
    GOODMT_HWINFO = 1,
#define GOODMT_HWINFO GOODMT_HWINFO

    /** 查询磁带状态。*/
    GOODMT_STATUS = 2,
#define GOODMT_STATUS GOODMT_STATUS

    /** 倒带。*/
    GOODMT_REWIND = 3,
#define GOODMT_REWIND GOODMT_REWIND

    /** 加载。*/
    GOODMT_LOAD = 4,
#define GOODMT_LOAD GOODMT_LOAD

    /** 卸载。*/
    GOODMT_UNLOAD = 5,
#define GOODMT_UNLOAD GOODMT_UNLOAD

    /** 加锁。*/
    GOODMT_LOCK = 6,
#define GOODMT_LOCK GOODMT_LOCK

    /** 解锁。*/
    GOODMT_UNLOCK = 7,
#define GOODMT_UNLOCK GOODMT_UNLOCK

    /** 读取磁头位置(逻辑)。*/
    GOODMT_READ_POS = 8,
#define GOODMT_READ_POS GOODMT_READ_POS

    /** 移动磁头位置(逻辑)。*/
    GOODMT_SEEK_POS = 9,
#define GOODMT_SEEK_POS GOODMT_SEEK_POS

    /** 写FILEMARK。*/
    GOODMT_WRITE_FILEMARK = 10
#define GOODMT_WRITE_FILEMARK GOODMT_WRITE_FILEMARK
};

void _goodmt_print_usage(good_tree_t *args, int only_version)
{
    char name[NAME_MAX] = {0};

    /*Clear errno.*/
    errno = 0;

    good_proc_basename(name);

#ifdef BUILD_VERSION_DATETIME
    fprintf(stderr, "\n%s Build %s\n", name, BUILD_VERSION_DATETIME);
#endif //BUILD_VERSION_DATETIME

    fprintf(stderr, "\n%s Version %d.%d\n", name, GOOD_VERSION_MAJOR, GOOD_VERSION_MINOR);

    if (only_version)
        return;

    fprintf(stderr, "\nOptions:\n");

    fprintf(stderr, "\n\t--help\n");
    fprintf(stderr, "\t\tShow this help message and exit.\n");

    fprintf(stderr, "\n\t--version\n");
    fprintf(stderr, "\t\tOutput version information and exit.\n");

    fprintf(stderr, "\n\t--dev < FILE >\n");
    fprintf(stderr, "\t\tBlock SCSI device.\n");

    fprintf(stderr, "\n\t--pos-block < NUMBER >\n");
    fprintf(stderr, "\t\tLogical object identifier.\n");

    fprintf(stderr, "\n\t--pos-part < NUMBER >\n");
    fprintf(stderr, "\t\tPartition identifier. default: 0\n");

    fprintf(stderr, "\n\t--filemarks < NUMBER >\n");
    fprintf(stderr, "\t\tLogical object numbers. default: 1\n");

    fprintf(stderr, "\n\t--cmd < NUMBER >\n");
    fprintf(stderr, "\t\tCommand. default: %d\n", GOODMT_STATUS);

    fprintf(stderr, "\n\t\t%d: Report driver baseinfo.\n", GOODMT_HWINFO);
    fprintf(stderr, "\t\t%d: Report medium baseinfo.\n", GOODMT_STATUS);
    fprintf(stderr, "\t\t%d: Rewind.\n", GOODMT_REWIND);
    fprintf(stderr, "\t\t%d: Load.\n", GOODMT_LOAD);
    fprintf(stderr, "\t\t%d: Unload.\n", GOODMT_UNLOAD);
    fprintf(stderr, "\t\t%d: Lock.\n", GOODMT_LOCK);
    fprintf(stderr, "\t\t%d: Unlock.\n", GOODMT_UNLOCK);
    fprintf(stderr, "\t\t%d: Read position.\n", GOODMT_READ_POS);
    fprintf(stderr, "\t\t%d: Seek position.\n", GOODMT_SEEK_POS);
    fprintf(stderr, "\t\t%d: Write filemark.\n", GOODMT_WRITE_FILEMARK);
}


static const good_scsi_sense_info senseinfo_dicts[] = {
    /*KEY=0x00*/
    {0x00, 0x00, 0x00, "No Sense"},
    /*KEY=0x01*/
    {0x01, 0x00, 0x00, "Recovered Error"},
    /*KEY=0x02*/
    {0x02, 0x00, 0x00, "Not Ready"},
    {0x02, 0x30, 0x03, "Cleaning in progess"},
    {0x02, 0x30, 0x07, "Cleaning failure"},
    {0x02, 0x3a, 0x00, "Medium not present"},
    /*KEY=0x03*/
    {0x03, 0x00, 0x00, "Medium Error"},
    /*KEY=0x04*/
    {0x04, 0x00, 0x00, "Hardware Error"},
    /*KEY=0x05*/
    {0x05, 0x00, 0x00, "Illegal Request"},
    /*KEY=0x06*/
    {0x06, 0x00, 0x00, "Unit Attention"},
    /*KEY=0x07*/
    {0x07, 0x00, 0x00, "Data Protect"},
    /*KEY=0x08*/
    {0x08, 0x00, 0x00, "Blank Check"},
    {0x08, 0x00, 0x05, "End of data"},
    {0x08, 0x14, 0x03, "New tape"},
    /*KEY=0x0b*/
    {0x0b, 0x00, 0x00, "Command Aborted"},
    /*KEY=0x0d*/
    {0x0d, 0x00, 0x00, "Volume Overflow"}
};

void _goodmt_printf_sense(good_scsi_io_stat *stat)
{
    uint8_t key = 0, asc = 0, ascq = 0;
    const char *msg_p = "Unknown";

    key = good_scsi_sense_key(stat->sense);
    asc = good_scsi_sense_code(stat->sense);
    ascq = good_scsi_sense_qualifier(stat->sense);

    for (size_t i = 0; i < GOOD_ARRAY_SIZE(senseinfo_dicts); i++)
    {
        if (senseinfo_dicts[i].key != key)
            continue;

        msg_p = senseinfo_dicts[i].msg;

        if (senseinfo_dicts[i].asc != asc || senseinfo_dicts[i].ascq != ascq)
            continue;

        msg_p = senseinfo_dicts[i].msg;
        break;
    }


    syslog(LOG_INFO, "Sense(KEY=%02X,ASC=%02X,ASCQ=%02X): %s.", key, asc, ascq, msg_p);
}

void _goodmt_report_status(good_tree_t *args,int fd)
{
    good_scsi_io_stat stat = {0};
    good_allocator_t *attr_p[6] = {NULL};
    int chk;

    attr_p[0] = good_mt_read_attribute(fd,0,0x0000,3000,&stat);
    if(!attr_p[0] || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    good_endian_ntoh(attr_p[0]->pptrs[GOOD_MT_ATTR_VALUE],GOOD_PTR2U16(attr_p[0]->pptrs[GOOD_MT_ATTR_LENGTH],0));
    fprintf(stdout,"Remaining_Capacity: %lu\n",GOOD_PTR2U64(attr_p[0]->pptrs[GOOD_MT_ATTR_VALUE], 0));
    
    attr_p[1] = good_mt_read_attribute(fd,0,0x0001,3000,&stat);
    if(!attr_p[1] || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    good_endian_ntoh(attr_p[1]->pptrs[GOOD_MT_ATTR_VALUE],GOOD_PTR2U16(attr_p[1]->pptrs[GOOD_MT_ATTR_LENGTH],0));
    fprintf(stdout,"Maximum_Capacity: %lu\n",GOOD_PTR2U64(attr_p[1]->pptrs[GOOD_MT_ATTR_VALUE], 0));
    
    attr_p[2] = good_mt_read_attribute(fd,0,0x0400,3000,&stat);
    if(!attr_p[2] || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Manufacturer: %s\n",attr_p[2]->pptrs[GOOD_MT_ATTR_VALUE]);
    
    attr_p[3] = good_mt_read_attribute(fd,0,0x0401,3000,&stat);
    if(!attr_p[3] || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Serial_Number: %s\n",attr_p[3]->pptrs[GOOD_MT_ATTR_VALUE]);
    

    attr_p[4] = good_mt_read_attribute(fd,0,0x0405,3000,&stat);
    if(!attr_p[4] || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Density: %s\n",good_mt_density2string(GOOD_PTR2U8(attr_p[4]->pptrs[GOOD_MT_ATTR_VALUE], 0)));
    

    attr_p[5] = good_mt_read_attribute(fd,0,0x0806,3000,&stat);
    if(!attr_p[5] || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Barcode: %s\n",attr_p[5]->pptrs[GOOD_MT_ATTR_VALUE]);
    
   
    /*No error.*/
    goto final;

print_sense:

    _goodmt_printf_sense(&stat);

final:

    for(size_t i = 0;i<GOOD_ARRAY_SIZE(attr_p);i++)
    {
        if(!attr_p[i])
            continue;

        good_allocator_unref(&attr_p[i]);
    }
}

void _goodmt_read_pos(good_tree_t *args,int fd)
{    
    good_scsi_io_stat stat = {0};
    uint64_t pos_block = -1, pos_file = -1;
    uint32_t pos_part = -1;
    int chk;

    chk = good_mt_read_position(fd,&pos_block,&pos_file,&pos_part,3000,&stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Block: %lu\n",pos_block);
    fprintf(stdout,"File: %lu\n",pos_file);
    fprintf(stdout,"Partition: %u\n",pos_part);

    /*No error.*/
    goto final;

print_sense:

    _goodmt_printf_sense(&stat);

final:

    return;
}

void _goodmt_seek_pos(good_tree_t *args,int fd)
{    
    good_scsi_io_stat stat = {0};
    uint64_t pos_block = INTMAX_MAX;
    uint32_t pos_part = 0;
    int chk;

    pos_block = good_option_get_long(args, "--pos-block", 0, INTMAX_MAX);
    pos_part = good_option_get_int(args, "--pos-part", 0, 0);

    chk = good_mt_locate(fd,1,pos_part,pos_block,1800*1000,&stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);

        /*No error.*/
    goto final;

print_sense:

    _goodmt_printf_sense(&stat);

final:

    return;
}

void _goodmt_work(good_tree_t *args)
{
    good_scsi_io_stat stat = {0};
    uint8_t type = 0;
    char vendor[32] = {0};
    char product[64] = {0};
    char sn[64] = {0};
    int fd = -1;
    const char *dev_p = NULL;
    int filemarks = 0;
    int cmd = 0;
    int chk;

    /*Clear errno.*/
    errno = 0;

    dev_p = good_option_get(args, "--dev", 0, "");
    cmd = good_option_get_int(args, "--cmd", 0, GOODMT_STATUS);

    if (access(dev_p, F_OK) != 0)
    {
        syslog(LOG_WARNING, "'%s' No such device.", dev_p);
        goto final;
    }

    fd = good_open(dev_p, 1, 1, 0);
    if (fd < 0)
    {
        syslog(LOG_WARNING, "'%s' %s.",dev_p,strerror(errno));
        goto final;
    }

    chk = good_scsi_inquiry_standard(fd, &type, vendor, product, 3000, &stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);

    if (type != TYPE_TAPE)
    {
        syslog(LOG_WARNING, "'%s' Not Sequential-Access(tape).", dev_p);
        GOOD_ERRNO_AND_GOTO1(EINVAL,final);
    }

    chk = good_scsi_inquiry_serial(fd, NULL, sn, 3000, &stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    if(cmd == GOODMT_HWINFO)
    {
        fprintf(stdout,"Vendor: %s\n",vendor);
        fprintf(stdout,"Product: %s\n",product);
        fprintf(stdout,"Serial_Number: %s\n",sn);
    }
    else if (cmd == GOODMT_STATUS)
    {
        _goodmt_report_status(args, fd);
    }
    else if (cmd == GOODMT_REWIND)
    {
        chk = good_mt_rewind(fd,0);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == GOODMT_LOAD)
    {
        chk = good_mt_load(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == GOODMT_UNLOAD)
    {
        chk = good_mt_unload(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == GOODMT_LOCK)
    {
        chk = good_mt_lock(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == GOODMT_UNLOCK)
    {
        chk = good_mt_unlock(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == GOODMT_READ_POS)
    {
        _goodmt_read_pos(args,fd);
    }
    else if (cmd == GOODMT_SEEK_POS)
    {
        _goodmt_seek_pos(args,fd);
    }
    else if (cmd == GOODMT_WRITE_FILEMARK)
    {
        filemarks = good_option_get_int(args, "--filemarks", 0, 1);
        chk = good_mt_writefm(fd,filemarks);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else
    {
        syslog(LOG_WARNING, "Not supported.");
        GOOD_ERRNO_AND_GOTO1(EINVAL, final);
    }

    /*No error.*/
    goto final;

print_sense:

    _goodmt_printf_sense(&stat);

final:

    good_closep(&fd);
}

int main(int argc, char **argv)
{
    good_tree_t *args;

    args = good_tree_alloc3(1);
    if (!args)
        goto final;

    good_getargs(args, argc, argv, "--");

    good_openlog(NULL, LOG_INFO, 1);

    if (good_option_exist(args, "--help"))
    {
        _goodmt_print_usage(args, 0);
    }
    else if (good_option_exist(args, "--version"))
    {
        _goodmt_print_usage(args, 1);
    }
    else
    {
        _goodmt_work(args);
    }

final:

    good_tree_free(&args);

    return errno;
}