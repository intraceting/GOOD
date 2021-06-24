/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "getargs.h"

void abtk_getargs(abtk_tree_t *opt, int argc, char *argv[],
                  const char *prefix)
{
    size_t prefix_len = 0;
    const char *it_key = NULL;

    assert(opt != NULL && argc > 0 && argv != NULL && prefix != NULL);

    assert(argv[0] != NULL && argv[0][0] != '\0' && prefix[0] != '\0');

    prefix_len = strlen(prefix);
    it_key = prefix;

    for (int i = 0; i < argc;)
    {
        if (abtk_strncmp(argv[i], prefix, prefix_len, 1) != 0)
        {
            abtk_option_set(opt, it_key, argv[i++]);
        }
        else
        {
            abtk_option_set(opt, it_key = argv[i++], NULL);
        }
    }
}

static ssize_t _abtk_getargs_getline(FILE *fp, char **line, size_t *len, uint8_t delim, char note)
{
    char *line_p = NULL;
    ssize_t chk = -1;

    while ((chk = getdelim(line, len, delim, fp)) != -1)
    {
        line_p = *line;

        if (*line_p == '\0' || *line_p == note || iscntrl(*line_p))
            continue;
        else
            break;
    }

    return chk;
}

int _abtk_getargs_valtrim(int c)
{
    return (iscntrl(c) || (c == '\"') || (c == '\''));
}

void abtk_getargs_fp(abtk_tree_t *opt, FILE *fp, uint8_t delim, char note,
                     const char *argv0, const char *prefix)
{
    size_t prefix_len = 0;
    const char *it_key = NULL;
    char *line = NULL;
    size_t len = 0;
    size_t rows = 0;
    char *key_p = NULL;
    char *val_p = NULL;

    assert(opt != NULL && fp != NULL);

    if (prefix != NULL)
    {
        prefix_len = strlen(prefix);
        it_key = prefix;

        if (argv0)
            abtk_option_set(opt, it_key, argv0);

        while (_abtk_getargs_getline(fp, &line, &len, delim, note) != -1)
        {
            /* 去掉字符串两端所有控制字符。 */
            abtk_strtrim(line, iscntrl, 2);

            if (abtk_strncmp(line, prefix, prefix_len, 1) != 0)
            {
                abtk_option_set(opt, it_key, line);
            }
            else
            {
                if (it_key != prefix)
                    abtk_heap_freep((void **)&it_key);

                it_key = abtk_heap_clone(line, len + 1);
                if (!it_key)
                    break;

                abtk_option_set(opt, it_key, NULL);
            }
        }
    }
    else
    {
        while (_abtk_getargs_getline(fp, &line, &len, delim, note) != -1)
        {
            /* Find key.*/
            key_p = line;

            /* Find Value.*/
            val_p = strchr(line, '=');
            if (!val_p)
                val_p = strchr(line, ':');

            if (val_p)
            {
                *val_p = '\0'; // for key end.
                val_p += 1;

                /* 去掉value两端所有控制字符、双引号、单引号。 */
                abtk_strtrim(val_p, _abtk_getargs_valtrim, 2);
            }

            /* 去掉key两端所有控制字符。 */
            abtk_strtrim(key_p, iscntrl, 2);

            abtk_option_set(opt, key_p, val_p);
        }
    }

    /*不要忘记释放这两块内存，不然可能会有内存泄漏的风险。 */
    if (line)
        free(line);
    if (it_key != prefix)
        abtk_heap_freep((void **)&it_key);
}

void abtk_getargs_file(abtk_tree_t *opt, const char *file, uint8_t delim, char note,
                       const char *argv0, const char *prefix)
{
    FILE *fp = NULL;

    assert(opt != NULL && file != NULL);

    fp = fopen(file, "r");
    if (!fp)
        return;

    abtk_getargs_fp(opt, fp, delim, note, argv0, prefix);

    fclose(fp);
}

void abtk_getargs_text(abtk_tree_t *opt, const char *text, size_t len, uint8_t delim, char note,
                       const char *argv0, const char *prefix)
{
    FILE *fp = NULL;

    assert(opt != NULL && text != NULL && len > 0);

    fp = fmemopen((char *)text, len, "r");
    if (!fp)
        return;

    abtk_getargs_fp(opt, fp, delim, note, argv0, prefix);

    fclose(fp);
}