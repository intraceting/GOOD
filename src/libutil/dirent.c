/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "dirent.h"


void good_dirscan(good_tree_t *tree, const char *path, size_t deep,int onefs)
{
    DIR *f_dir = NULL;
    struct dirent *c_dir = NULL;
    struct stat f_attr;
    struct stat c_attr;
    good_tree_t *node = NULL;

    assert(tree && path);

    if (lstat(path, &f_attr) == -1)
        return;

    if (!S_ISDIR(f_attr.st_mode))
        GOOD_ERRNO_AND_RETURN0(ENOTDIR);

    f_dir = opendir(path);
    if (!f_dir)
        return;

    while (c_dir = readdir(f_dir))
    {
        if (good_strcmp(c_dir->d_name, ".", 1) == 0 || good_strcmp(c_dir->d_name, "..", 1) == 0)
            continue;

        node = good_tree_alloc3(PATH_MAX);
        if (!node)
            break;

        good_dirdir(node->alloc->pptrs[0], path);
        good_dirdir(node->alloc->pptrs[0], c_dir->d_name);
      
        /*
         * 加入到树节点。
        */
        good_tree_insert2(tree, node, 0);

        /*
         * node->d_type 有些文件系统有BUG未设置有效值，因此不能直接使用，这里用替待方案。
         */
        if (lstat(node->alloc->pptrs[0], &c_attr) == -1)
        {
            good_tree_free(&node);
            break;
        }

        /*
         * 如果不是目录，下面的代码不需要执行。
        */
        if (!S_ISDIR(c_attr.st_mode))
            continue;

        /*
         * 递归深度。
        */
        if (deep <= 0)
            continue;

        /*
         * 同一个文件系统。
        */
        if(onefs && f_attr.st_dev != c_attr.st_dev)
            continue;

        /*
         * 递归
        */
        good_dirscan(node, node->alloc->pptrs[0], deep - 1, onefs);
    }

    if (f_dir)
        closedir(f_dir);
}