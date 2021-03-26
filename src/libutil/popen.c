/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "popen.h"

int _good_popen_setenv(size_t deep, const good_tree_t *node, void *opaque)
{
    if (deep <= 1)
        return 1;

    if (setenv(GOOD_PTR2PTR(char, node->buf->data[GOOD_MAP_KEY], 0), GOOD_PTR2PTR(char, node->buf->data[GOOD_MAP_VALUE], 0), 1) != 0)
        return -1;

    return 1;
}

pid_t good_popen(const char *cmd, int *stdin_fd, int *stdout_fd, int *stderr_fd, const good_map_t *env)
{
    pid_t child = -1;
    int out2in_fd[2] = {-1, -1};
    int in2out_fd[2] = {-1, -1};
    int in2err_fd[2] = {-1, -1};

    assert(cmd);

    if (pipe(out2in_fd) != 0)
        goto final;

    if (pipe(in2out_fd) != 0)
        goto final;

    if (pipe(in2err_fd) != 0)
        goto final;

    child = fork();
    if (child < 0)
        goto final;

    if (child == 0)
    {
        if (env)
        {
            good_tree_iterator it = {0};
            it.dump_cb = _good_popen_setenv;

            good_tree_scan(env->table, &it);
        }

        good_closep(&out2in_fd[1]);
        if (stdin_fd)
            dup2(out2in_fd[0], STDIN_FILENO);
        else
            good_open2(STDIN_FILENO, "/dev/null", 0, 0, 0);
        good_closep(&out2in_fd[0]);
        /**/
        good_closep(&in2out_fd[0]);
        if (stdout_fd)
            dup2(in2out_fd[1], STDOUT_FILENO);
        else
            good_open2(STDOUT_FILENO, "/dev/null", 1, 0, 0);
        good_closep(&in2out_fd[1]);
        /**/
        good_closep(&in2err_fd[0]);
        if (stderr_fd)
            dup2(in2err_fd[1], STDERR_FILENO);
        else
            good_open2(STDERR_FILENO, "/dev/null", 1, 0, 0);
        good_closep(&in2err_fd[1]);

        /*
             * 这个基本都支持。
            */
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);

        /*Maybe it will never be here.*/
        _exit(127);
    }
    else
    {
        /*
        * 关闭不需要的句柄。
        */
        good_closep(&out2in_fd[0]);
        good_closep(&in2out_fd[1]);
        good_closep(&in2err_fd[1]);

        if (!stdin_fd)
            good_closep(&out2in_fd[1]);
        else
            *stdin_fd = out2in_fd[1];

        if (!stdout_fd)
            good_closep(&in2out_fd[0]);
        else
            *stdout_fd = in2out_fd[0];

        if (!stderr_fd)
            good_closep(&in2err_fd[0]);
        else
            *stderr_fd = in2err_fd[0];
    }

final:

    if (child < 0)
    {
        good_closep(&out2in_fd[0]);
        good_closep(&out2in_fd[1]);
        good_closep(&in2out_fd[0]);
        good_closep(&in2out_fd[1]);
        good_closep(&in2err_fd[0]);
        good_closep(&in2err_fd[1]);
    }

    return child;
}