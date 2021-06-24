/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_DIRENT_H
#define ABTKUTIL_DIRENT_H

#include "general.h"
#include "tree.h"

/**
 * 文件和目录项的字段。
*/
enum _abtk_dirent_field
{
    /**
     * 名字(包括路径) 字段索引。
    */
   ABTK_DIRENT_NAME = 0,
#define ABTK_DIRENT_NAME    ABTK_DIRENT_NAME

    /**
     * 状态 字段索引。
    */
   ABTK_DIRENT_STAT = 1
#define ABTK_DIRENT_STAT    ABTK_DIRENT_STAT

};

/**
 * 目录扫描。
 * 
 * 扫描的结果会自动生成一个颗“树”。
 * 
 * @warning 如果目录和文件较多，则需要较多的内存。
 * 
 * @param depth 遍历深度。0 只遍历当前目录，>= 1 遍历多级目录。
 * @param onefs 0 不辨别文件系统是否相同，!0 只在同一个文件系统中遍历。
 * 
*/
void abtk_dirscan(abtk_tree_t *father,size_t depth, int onefs);

#endif //ABTKUTIL_DIRENT_H