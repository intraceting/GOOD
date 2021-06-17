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
#include "goodutil/mtx.h"

/**/
enum _goodmtx_cmd
{
    /** 枚举磁带库所有元素。*/
    GOODMTX_STATUS = 1,
#define GOODMTX_STATUS GOODMTX_STATUS

    /** 移动磁带。*/
    GOODMTX_MOVE = 2
#define GOODMTX_MOVE GOODMTX_MOVE

};

/**/
struct _goodmtx_senseinfo
{   
    /** */
    uint8_t key;

    /** */
    uint8_t asc;

    /** */
    uint8_t ascq;

    /** */
    const char *msg;
}; 

void _goodmtx_print_usage(good_tree_t *args, int only_version)
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
    fprintf(stderr, "\t\tGeneric SCSI device.\n");

    fprintf(stderr, "\n\t--src < ADDRESS >\n");
    fprintf(stderr, "\t\tSource Element Address.\n");

    fprintf(stderr, "\n\t--dst < ADDRESS >\n");
    fprintf(stderr, "\t\tDestination Element Address.\n");

    fprintf(stderr, "\n\t--cmd < NUMBER >\n");
    fprintf(stderr, "\t\tCommand. default: %d\n", GOODMTX_STATUS);

    fprintf(stderr, "\n\t\t%d: Report elements.\n", GOODMTX_STATUS);
    fprintf(stderr, "\t\t%d: Move Medium.\n", GOODMTX_MOVE);
}

static const struct _goodmtx_senseinfo senseinfo_keys[] = {
    {0x01, 0x00, 0x00, "Recovered Error"},
    {0x02, 0x00, 0x00, "Not Ready"},
    {0x03, 0x00, 0x00, "Medium Error"},
    {0x04, 0x00, 0x00, "Hardware Error"},
    {0x05, 0x00, 0x00, "Illegal Request"},
    {0x06, 0x00, 0x00, "Unit Attention"},
    {0x0b, 0x00, 0x00, "Command Aborted"},
    {0,0,0,""},
};

static const struct _goodmtx_senseinfo senseinfo_details[] = {
    {0x05, 0x21, 0x01, "Invalid element address"},
    {0x05, 0x24, 0x00, "Invalid field CDB or Invalid element address"},
    {0x05, 0x3b, 0x0d, "Medium destination element full"},
    {0x05, 0x3b, 0x0e, "Medium source element empty"},
    {0x05, 0x53, 0x02, "Library media removal prevented state set"},
    {0x05, 0x53, 0x03, "Drive media removal prevented state set"},
    {0x05, 0x44, 0x80, "Bad status library controller"},
    {0x05, 0x44, 0x81, "Source not ready"},
    {0x05, 0x44, 0x82, "Destination not ready"},
    {0,0,0,""},
};

void _goodmtx_printf_sense(good_scsi_io_stat *stat)
{
    uint8_t key = 0, asc = 0, ascq = 0;
    const char *msg_p = "Unknown";

    key = good_scsi_sense_key(stat->sense);
    asc = good_scsi_sense_code(stat->sense);
    ascq = good_scsi_sense_qualifier(stat->sense);

    /**/
    for (size_t i = 0; i < GOOD_ARRAY_SIZE(senseinfo_details); i++)
    {
        if (senseinfo_details[i].key != key ||
            senseinfo_details[i].asc != asc ||
            senseinfo_details[i].ascq != ascq)
            continue;

        msg_p = senseinfo_details[i].msg;
        break;
    }

    if(msg_p)
        goto final;

    /*No details, translate KEY.*/
    for (size_t i = 0; i < GOOD_ARRAY_SIZE(senseinfo_keys); i++)
    {
        if(senseinfo_keys[i].key != key)
            continue;
        
        msg_p = senseinfo_keys[i].msg;
        break;
    }

final:

    syslog(LOG_INFO, "Sense(KEY=%02X,ASC=%02X,ASCQ=%02X): %s.", key, asc, ascq, msg_p);
}

int _goodmtx_printf_elements_cb(size_t deep, good_tree_t *node, void *opaque)
{
    if (deep == 0)
    {
        good_tree_fprintf(stdout, deep, node, "%s\n", node->alloc->pptrs[0]);
    }
    else
    {
        good_tree_fprintf(stdout, deep, node, "%-6hu\t|%-2hhu\t|%-2hhu\t|%-10s\t|%-10s\t|\n",
                          GOOD_PTR2U16(node->alloc->pptrs[GOOD_MTX_ELEMENT_ADDR], 0),
                          GOOD_PTR2U8(node->alloc->pptrs[GOOD_MTX_ELEMENT_TYPE], 0),
                          GOOD_PTR2U8(node->alloc->pptrs[GOOD_MTX_ELEMENT_ISFULL], 0),
                          node->alloc->pptrs[GOOD_MTX_ELEMENT_BARCODE],
                          node->alloc->pptrs[GOOD_MTX_ELEMENT_DVCID]);
    }

    return 1;
}

void _goodmtx_printf_elements(good_tree_t *root)
{
    good_tree_iterator_t it = {0, _goodmtx_printf_elements_cb, NULL};
    good_tree_scan(root, &it);
}

int _goodmtx_find_changer_cb(size_t deep, good_tree_t *node, void *opaque)
{
    uint16_t *t_p = (uint16_t *)opaque;

    if (deep == 0)
        return 1;

    if (GOOD_PTR2U8(node->alloc->pptrs[GOOD_MTX_ELEMENT_TYPE], 0) != GOOD_MXT_ELEMENT_CHANGER)
        return 1;

    *t_p = GOOD_PTR2U16(node->alloc->pptrs[GOOD_MTX_ELEMENT_ADDR], 0);

    return -1;
}

uint16_t _goodmtx_find_changer(good_tree_t *root)
{
    uint16_t t = 0;

    good_tree_iterator_t it = {0, _goodmtx_find_changer_cb, &t};
    good_tree_scan(root, &it);

    return t;
}

void _goodmtx_move_medium(good_tree_t *args, int fd, good_tree_t *root)
{
    good_scsi_io_stat stat = {0};
    int t = 0, s = 65536, d = 65536;
    int chk;

    t = _goodmtx_find_changer(root);
    s = good_option_get_int(args, "--src", 0, 65536);
    d = good_option_get_int(args, "--dst", 0, 65536);

    chk = good_mtx_move_medium(fd, t, s, d, 1800 * 1000, &stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EINVAL,print_sense);

    /*No error.*/
    goto final;

print_sense:

    _goodmtx_printf_sense(&stat);

final:

    return;
}

void _goodmtx_work(good_tree_t *args)
{
    good_scsi_io_stat stat = {0};
    good_tree_t *root = NULL;
    uint8_t type = 0;
    char vendor[32] = {0};
    char product[64] = {0};
    char sn[64] = {0};
    int fd = -1;
    const char *dev_p = NULL;
    int cmd = 0;
    int chk;

    /*Clear errno.*/
    errno = 0;

    dev_p = good_option_get(args, "--dev", 0, "");
    cmd = good_option_get_int(args, "--cmd", 0, GOODMTX_STATUS);

    if (access(dev_p, F_OK) != 0)
    {
        syslog(LOG_WARNING, "'%s' No such device.", dev_p);
        goto final;
    }

    root = good_tree_alloc3(200);
    if (!root)
        goto final;

    fd = good_open(dev_p, 0, 0, 0);
    if (fd < 0)
    {
        syslog(LOG_WARNING, "'%s' %s.",dev_p,strerror(errno));
        goto final;
    }

    chk = good_scsi_inquiry_standard(fd, &type, vendor, product, 3000, &stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);

    if (type != TYPE_MEDIUM_CHANGER)
    {
        syslog(LOG_WARNING, "'%s' Not Medium Changer.", dev_p);
        GOOD_ERRNO_AND_GOTO1(EINVAL,final);
    }

    chk = good_scsi_inquiry_serial(fd, NULL, sn, 3000, &stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);

    snprintf(root->alloc->pptrs[0], root->alloc->sizes[0], "%s(%s-%s)", sn, vendor, product);

    chk = good_mtx_inquiry_element_status(root, fd, -1, &stat);
    if (chk != 0 || stat.status != GOOD)
        GOOD_ERRNO_AND_GOTO1(EPERM,print_sense);

    if (cmd == GOODMTX_STATUS)
    {
        _goodmtx_printf_elements(root);
    }
    else if (cmd == GOODMTX_MOVE)
    {
        _goodmtx_move_medium(args, fd, root);
    }
    else
    {
        syslog(LOG_WARNING, "Not supported.");
        GOOD_ERRNO_AND_GOTO1(EINVAL,final);
    }

    /*No error.*/
    goto final;

print_sense:

    _goodmtx_printf_sense(&stat);

final:

    good_closep(&fd);
    good_tree_free(&root);
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
        _goodmtx_print_usage(args, 0);
    }
    else if (good_option_exist(args, "--version"))
    {
        _goodmtx_print_usage(args, 1);
    }
    else
    {
        _goodmtx_work(args);
    }

final:

    good_tree_free(&args);

    return errno;
}