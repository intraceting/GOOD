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
#include "abtkutil/getargs.h"
#include "abtkutil/scsi.h"
#include "abtkutil/mt.h"

/**/
enum _abtkmt_cmd
{
    /** 查询驱动器信息。*/
    ABTKMT_HWINFO = 1,
#define ABTKMT_HWINFO ABTKMT_HWINFO

    /** 查询磁带状态。*/
    ABTKMT_STATUS = 2,
#define ABTKMT_STATUS ABTKMT_STATUS

    /** 倒带。*/
    ABTKMT_REWIND = 3,
#define ABTKMT_REWIND ABTKMT_REWIND

    /** 加载。*/
    ABTKMT_LOAD = 4,
#define ABTKMT_LOAD ABTKMT_LOAD

    /** 卸载。*/
    ABTKMT_UNLOAD = 5,
#define ABTKMT_UNLOAD ABTKMT_UNLOAD

    /** 加锁。*/
    ABTKMT_LOCK = 6,
#define ABTKMT_LOCK ABTKMT_LOCK

    /** 解锁。*/
    ABTKMT_UNLOCK = 7,
#define ABTKMT_UNLOCK ABTKMT_UNLOCK

    /** 读取磁头位置(逻辑)。*/
    ABTKMT_READ_POS = 8,
#define ABTKMT_READ_POS ABTKMT_READ_POS

    /** 移动磁头位置(逻辑)。*/
    ABTKMT_SEEK_POS = 9,
#define ABTKMT_SEEK_POS ABTKMT_SEEK_POS

    /** 写FILEMARK。*/
    ABTKMT_WRITE_FILEMARK = 10
#define ABTKMT_WRITE_FILEMARK ABTKMT_WRITE_FILEMARK
};

void _abtkmt_print_usage(abtk_tree_t *args, int only_version)
{
    char name[NAME_MAX] = {0};

    /*Clear errno.*/
    errno = 0;

    abtk_proc_basename(name);

#ifdef BUILD_VERSION_DATETIME
    fprintf(stderr, "\n%s Build %s\n", name, BUILD_VERSION_DATETIME);
#endif //BUILD_VERSION_DATETIME

    fprintf(stderr, "\n%s Version %d.%d\n", name, ABTK_VERSION_MAJOR, ABTK_VERSION_MINOR);

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
    fprintf(stderr, "\t\tCommand. default: %d\n", ABTKMT_STATUS);

    fprintf(stderr, "\n\t\t%d: Report driver baseinfo.\n", ABTKMT_HWINFO);
    fprintf(stderr, "\t\t%d: Report medium baseinfo.\n", ABTKMT_STATUS);
    fprintf(stderr, "\t\t%d: Rewind.\n", ABTKMT_REWIND);
    fprintf(stderr, "\t\t%d: Load.\n", ABTKMT_LOAD);
    fprintf(stderr, "\t\t%d: Unload.\n", ABTKMT_UNLOAD);
    fprintf(stderr, "\t\t%d: Lock.\n", ABTKMT_LOCK);
    fprintf(stderr, "\t\t%d: Unlock.\n", ABTKMT_UNLOCK);
    fprintf(stderr, "\t\t%d: Read position.\n", ABTKMT_READ_POS);
    fprintf(stderr, "\t\t%d: Seek position.\n", ABTKMT_SEEK_POS);
    fprintf(stderr, "\t\t%d: Write filemark.\n", ABTKMT_WRITE_FILEMARK);
}

static struct _abtkmt_sense_dict
{   
    uint8_t key;
    uint8_t asc;
    uint8_t ascq;
    const char *msg;
}abtkmt_sense_dict[] = {
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

void _abtkmt_printf_sense(abtk_scsi_io_stat *stat)
{
    uint8_t key = 0, asc = 0, ascq = 0;
    const char *msg_p = "Unknown";

    key = abtk_scsi_sense_key(stat->sense);
    asc = abtk_scsi_sense_code(stat->sense);
    ascq = abtk_scsi_sense_qualifier(stat->sense);

    for (size_t i = 0; i < ABTK_ARRAY_SIZE(abtkmt_sense_dict); i++)
    {
        if (abtkmt_sense_dict[i].key != key)
            continue;

        msg_p = abtkmt_sense_dict[i].msg;

        if (abtkmt_sense_dict[i].asc != asc || abtkmt_sense_dict[i].ascq != ascq)
            continue;

        msg_p = abtkmt_sense_dict[i].msg;
        break;
    }


    syslog(LOG_INFO, "Sense(KEY=%02X,ASC=%02X,ASCQ=%02X): %s.", key, asc, ascq, msg_p);
}

void _abtkmt_report_status(abtk_tree_t *args,int fd)
{
    abtk_scsi_io_stat stat = {0};
    abtk_allocator_t *attr_p[6] = {NULL};
    int chk;

    attr_p[0] = abtk_mt_read_attribute(fd,0,0x0000,3000,&stat);
    if(!attr_p[0] || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    abtk_endian_ntoh(attr_p[0]->pptrs[ABTK_MT_ATTR_VALUE],ABTK_PTR2U16(attr_p[0]->pptrs[ABTK_MT_ATTR_LENGTH],0));
    fprintf(stdout,"Remaining_Capacity: %lu\n",ABTK_PTR2U64(attr_p[0]->pptrs[ABTK_MT_ATTR_VALUE], 0));
    
    attr_p[1] = abtk_mt_read_attribute(fd,0,0x0001,3000,&stat);
    if(!attr_p[1] || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    abtk_endian_ntoh(attr_p[1]->pptrs[ABTK_MT_ATTR_VALUE],ABTK_PTR2U16(attr_p[1]->pptrs[ABTK_MT_ATTR_LENGTH],0));
    fprintf(stdout,"Maximum_Capacity: %lu\n",ABTK_PTR2U64(attr_p[1]->pptrs[ABTK_MT_ATTR_VALUE], 0));
    
    attr_p[2] = abtk_mt_read_attribute(fd,0,0x0400,3000,&stat);
    if(!attr_p[2] || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Manufacturer: %s\n",attr_p[2]->pptrs[ABTK_MT_ATTR_VALUE]);
    
    attr_p[3] = abtk_mt_read_attribute(fd,0,0x0401,3000,&stat);
    if(!attr_p[3] || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Serial_Number: %s\n",attr_p[3]->pptrs[ABTK_MT_ATTR_VALUE]);
    

    attr_p[4] = abtk_mt_read_attribute(fd,0,0x0405,3000,&stat);
    if(!attr_p[4] || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Density: %s\n",abtk_mt_density2string(ABTK_PTR2U8(attr_p[4]->pptrs[ABTK_MT_ATTR_VALUE], 0)));
    

    attr_p[5] = abtk_mt_read_attribute(fd,0,0x0806,3000,&stat);
    if(!attr_p[5] || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Barcode: %s\n",attr_p[5]->pptrs[ABTK_MT_ATTR_VALUE]);
    
   
    /*No error.*/
    goto final;

print_sense:

    _abtkmt_printf_sense(&stat);

final:

    for(size_t i = 0;i<ABTK_ARRAY_SIZE(attr_p);i++)
    {
        if(!attr_p[i])
            continue;

        abtk_allocator_unref(&attr_p[i]);
    }
}

void _abtkmt_read_pos(abtk_tree_t *args,int fd)
{    
    abtk_scsi_io_stat stat = {0};
    uint64_t pos_block = -1, pos_file = -1;
    uint32_t pos_part = -1;
    int chk;

    chk = abtk_mt_read_position(fd,&pos_block,&pos_file,&pos_part,3000,&stat);
    if (chk != 0 || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    fprintf(stdout,"Block: %lu\n",pos_block);
    fprintf(stdout,"File: %lu\n",pos_file);
    fprintf(stdout,"Partition: %u\n",pos_part);

    /*No error.*/
    goto final;

print_sense:

    _abtkmt_printf_sense(&stat);

final:

    return;
}

void _abtkmt_seek_pos(abtk_tree_t *args,int fd)
{    
    abtk_scsi_io_stat stat = {0};
    uint64_t pos_block = INTMAX_MAX;
    uint32_t pos_part = 0;
    int chk;

    pos_block = abtk_option_get_long(args, "--pos-block", 0, INTMAX_MAX);
    pos_part = abtk_option_get_int(args, "--pos-part", 0, 0);

    chk = abtk_mt_locate(fd,1,pos_part,pos_block,1800*1000,&stat);
    if (chk != 0 || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);

        /*No error.*/
    goto final;

print_sense:

    _abtkmt_printf_sense(&stat);

final:

    return;
}

void _abtkmt_work(abtk_tree_t *args)
{
    abtk_scsi_io_stat stat = {0};
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

    dev_p = abtk_option_get(args, "--dev", 0, "");
    cmd = abtk_option_get_int(args, "--cmd", 0, ABTKMT_STATUS);

    if (access(dev_p, F_OK) != 0)
    {
        syslog(LOG_WARNING, "'%s' No such device.", dev_p);
        goto final;
    }

    fd = abtk_open(dev_p, 1, 1, 0);
    if (fd < 0)
    {
        syslog(LOG_WARNING, "'%s' %s.",dev_p,strerror(errno));
        goto final;
    }

    chk = abtk_scsi_inquiry_standard(fd, &type, vendor, product, 3000, &stat);
    if (chk != 0 || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);

    if (type != TYPE_TAPE)
    {
        syslog(LOG_WARNING, "'%s' Not Sequential-Access(tape).", dev_p);
        ABTK_ERRNO_AND_GOTO1(EINVAL,final);
    }

    chk = abtk_scsi_inquiry_serial(fd, NULL, sn, 3000, &stat);
    if (chk != 0 || stat.status != GOOD)
        ABTK_ERRNO_AND_GOTO1(EPERM,print_sense);
    
    if(cmd == ABTKMT_HWINFO)
    {
        fprintf(stdout,"Vendor: %s\n",vendor);
        fprintf(stdout,"Product: %s\n",product);
        fprintf(stdout,"Serial_Number: %s\n",sn);
    }
    else if (cmd == ABTKMT_STATUS)
    {
        _abtkmt_report_status(args, fd);
    }
    else if (cmd == ABTKMT_REWIND)
    {
        chk = abtk_mt_rewind(fd,0);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == ABTKMT_LOAD)
    {
        chk = abtk_mt_load(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == ABTKMT_UNLOAD)
    {
        chk = abtk_mt_unload(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == ABTKMT_LOCK)
    {
        chk = abtk_mt_lock(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == ABTKMT_UNLOCK)
    {
        chk = abtk_mt_unlock(fd);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else if (cmd == ABTKMT_READ_POS)
    {
        _abtkmt_read_pos(args,fd);
    }
    else if (cmd == ABTKMT_SEEK_POS)
    {
        _abtkmt_seek_pos(args,fd);
    }
    else if (cmd == ABTKMT_WRITE_FILEMARK)
    {
        filemarks = abtk_option_get_int(args, "--filemarks", 0, 1);
        chk = abtk_mt_writefm(fd,filemarks);
        if (chk != 0)
        {   
            syslog(LOG_WARNING, "'%s' %s.", dev_p, strerror(errno));
            goto final;
        }
    }
    else
    {
        syslog(LOG_WARNING, "Not supported.");
        ABTK_ERRNO_AND_GOTO1(EINVAL, final);
    }

    /*No error.*/
    goto final;

print_sense:

    _abtkmt_printf_sense(&stat);

final:

    abtk_closep(&fd);
}

int main(int argc, char **argv)
{
    abtk_tree_t *args;

    args = abtk_tree_alloc3(1);
    if (!args)
        goto final;

    abtk_getargs(args, argc, argv, "--");

    abtk_openlog(NULL, LOG_INFO, 1);

    if (abtk_option_exist(args, "--help"))
    {
        _abtkmt_print_usage(args, 0);
    }
    else if (abtk_option_exist(args, "--version"))
    {
        _abtkmt_print_usage(args, 1);
    }
    else
    {
        _abtkmt_work(args);
    }

final:

    abtk_tree_free(&args);

    return errno;
}