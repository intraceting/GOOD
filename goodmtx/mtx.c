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
    /**
     * 枚举磁带库所有元素。
    */
   GOODMTX_STATUS = 1,
#define GOODMTX_STATUS    GOODMTX_STATUS

    /**
     * 移动磁带。
    */
   GOODMTX_MOVE = 2
#define GOODMTX_MOVE    GOODMTX_MOVE

};

void _goodmtx_print_usage(good_tree_t *args,int only_version)
{
    char name[NAME_MAX] = {0};
    good_proc_basename(name);

#ifdef BUILD_VERSION_DATETIME
    fprintf(stderr, "\n%s Build %s\n", name, BUILD_VERSION_DATETIME);
#endif //BUILD_VERSION_DATETIME

    fprintf(stderr, "\n%s Version %d.%d\n", name, GOOD_VERSION_MAJOR, GOOD_VERSION_MINOR);

    if(only_version)
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
    fprintf(stderr, "\t\tCommand. default: %d\n",GOODMTX_STATUS);

    fprintf(stderr, "\n\t\t%d: Reports elements.\n",GOODMTX_STATUS);
    fprintf(stderr, "\t\t%d: Move Medium.\n",GOODMTX_MOVE);
}

void _goodmtx_printf_sense(good_scsi_io_stat *stat)
{
    syslog(LOG_INFO, "Sense: KEY=%02X,ASC=%02X,ASCQ=%02X",
           good_scsi_sense_key(stat->sense),
           good_scsi_sense_code(stat->sense),
           good_scsi_sense_qualifier(stat->sense));
}

int _goodmtx_printf_elements_cb(size_t deep, good_tree_t *node, void *opaque)
{
    if (deep == 0)
    {
        good_tree_fprintf(stdout, deep, node, "%s\n",node->alloc->pptrs[0]);
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
    good_tree_iterator_t it = {0,_goodmtx_printf_elements_cb,NULL};
    good_tree_scan(root,&it);
}

void _goodmtx_report_elements(good_tree_t *args)
{
    good_scsi_io_stat stat = {0};
    good_tree_t *root = NULL;
    uint8_t type = 0;
    char vendor[32] = {0};
    char product[64] = {0};
    char sn[64] = {0};
    int fd = -1;
    int chk;

    root = good_tree_alloc3(200);
    if(!root)
        goto final;
        
    fd = good_open(good_option_get(args,"--dev",0,""),0,0,0);
    if(fd <0)
        goto final;

    chk = good_scsi_inquiry_standard(fd,&type,vendor,product,3000,&stat);
    if (chk != 0)
        goto print_sense;

    chk = good_scsi_inquiry_serial(fd,NULL,sn,3000,&stat);
    if (chk != 0)
        goto print_sense;

    snprintf(root->alloc->pptrs[0],root->alloc->sizes[0],"%s(%s-%s)",sn,vendor,product);

    chk = good_mtx_inquiry_element_status(root, fd, -1, &stat);
    if (chk != 0)
        goto print_sense;

    _goodmtx_printf_elements(root);

    /*No error.*/
    goto final;

print_sense:

    _goodmtx_printf_sense(&stat);

final:

    good_closep(&fd);
    good_tree_free(&root);
}

int _goodmtx_find_changer_cb(size_t deep, good_tree_t *node, void *opaque)
{
    uint16_t *t_p = (uint16_t *)opaque;

    if (deep == 0)
        return 1;

    if(GOOD_PTR2U8(node->alloc->pptrs[GOOD_MTX_ELEMENT_TYPE],0) != GOOD_MXT_ELEMENT_CHANGER)
        return 1;


    *t_p = GOOD_PTR2U16(node->alloc->pptrs[GOOD_MTX_ELEMENT_ADDR],0);

    return -1;
}

uint16_t _goodmtx_find_changer(good_tree_t *root)
{
    uint16_t t = 0;

    good_tree_iterator_t it = {0,_goodmtx_find_changer_cb,&t};
    good_tree_scan(root,&it);

    return t;
}

void _goodmtx_move_medium(good_tree_t *args)
{
    good_scsi_io_stat stat = {0};
    good_tree_t *root = NULL;
    int fd = -1;
    int t = 0, s = 65536, d = 65536;
    int chk;

    root = good_tree_alloc(NULL);
    if(!root)
        goto final;
        
    fd = good_open(good_option_get(args,"--dev",0,""),0,0,0);
    if(fd <0)
        goto final;

    chk = good_mtx_inquiry_element_status(root, fd, -1, &stat);
    if (chk != 0)
        goto print_sense;

    t = _goodmtx_find_changer(root);
    s = good_option_get_int(args,"--src",0,65536);
    d = good_option_get_int(args,"--dst",0,65536);

    chk = good_mtx_move_medium(fd,t,s,d,1800*1000,&stat);
    if (chk != 0)
        goto print_sense;

    /*No error.*/
    goto final;

print_sense:

    _goodmtx_printf_sense(&stat);

final:

    good_closep(&fd);
    good_tree_free(&root);
}

void _goodmtx_work(good_tree_t *args)
{
    int cmd = good_option_get_int(args,"--cmd",0,GOODMTX_STATUS);

    if(cmd == GOODMTX_STATUS)
        _goodmtx_report_elements(args);
    else if(cmd == GOODMTX_MOVE)
        _goodmtx_move_medium(args);
    else
        syslog(LOG_WARNING,"Not supported.");
}

int main(int argc, char **argv)
{
    good_tree_t *args;

    args = good_tree_alloc(NULL);
    if(!args)
        goto final;

    good_getargs(args,argc,argv,"--");

    good_openlog(NULL,LOG_INFO,1);

    if (good_option_exist(args,"--help"))
    {
        _goodmtx_print_usage(args,0);
    }
    else if (good_option_exist(args,"--version"))
    {
        _goodmtx_print_usage(args,1);
    }
    else
    {
        _goodmtx_work(args);
    }
    
final:

    good_tree_free(&args);

    return errno;
}