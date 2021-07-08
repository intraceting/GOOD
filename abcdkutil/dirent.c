/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#include "dirent.h"

void abcdk_dirscan(abcdk_tree_t *father,size_t depth,int onefs)
{
    DIR *f_dir = NULL;
    struct dirent *c_dir = NULL;
    char *f_path = NULL;
    struct stat *f_stat = NULL;
    char *c_path = NULL;
    struct stat *c_stat = NULL;
    abcdk_tree_t *node = NULL;
    size_t sizes[2] = {PATH_MAX,sizeof(struct stat)};

    assert(father);
    assert(father->alloc->numbers >= 2);

    f_path = (char*)(father->alloc->pptrs[ABCDK_DIRENT_NAME]);
    f_stat = (struct stat *)(father->alloc->pptrs[ABCDK_DIRENT_STAT]);

    assert( f_path != NULL && *f_path != '\0');
    assert( f_stat != NULL);

    /* 递归进来的，查询一次即可。*/
    if(f_stat->st_nlink <= 0)
    {
        if (lstat(f_path, f_stat) == -1)
            return;
    }

    if (!S_ISDIR(f_stat->st_mode))
        ABCDK_ERRNO_AND_RETURN0(ENOTDIR);

    f_dir = opendir(f_path);
    if (!f_dir)
        return;

    while (c_dir = readdir(f_dir))
    {
        if (abcdk_strcmp(c_dir->d_name, ".", 1) == 0 || abcdk_strcmp(c_dir->d_name, "..", 1) == 0)
            continue;

        node = abcdk_tree_alloc2(sizes,2,0);
        if (!node)
            break;

        c_path = (char*)(node->alloc->pptrs[ABCDK_DIRENT_NAME]);
        c_stat = (struct stat *)(node->alloc->pptrs[ABCDK_DIRENT_STAT]);

        abcdk_dirdir(c_path, f_path);
        abcdk_dirdir(c_path, c_dir->d_name);
      
        /*加入到树节点。*/
        abcdk_tree_insert2(father, node, 0);

        /*node->d_type 有些文件系统有BUG未设置有效值，因此不能直接使用，这里用替待方案。 */
        if (lstat(c_path, c_stat) == -1)
        {
            abcdk_tree_free(&node);
            break;
        }
        
        /* 如果不是目录，下面的代码不需要执行。*/
        if (!S_ISDIR(c_stat->st_mode))
            continue;

        /* 递归深度。*/
        if (depth <= 0)
            continue;

        /* 同一个文件系统。*/
        if(onefs && f_stat->st_dev != c_stat->st_dev)
            continue;

        /* 递归。 */
        abcdk_dirscan(node, depth - 1, onefs);
    }

    if (f_dir)
        closedir(f_dir);
}