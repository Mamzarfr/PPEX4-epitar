#define _POSIX_C_SOURCE 200809L

#include "archive_utils.h"

#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tar_utils.h"

/**
 * @brief Fill a tar header
 * @param header Header to fill
 * @param path Path to the file/dir
 * @param stat stats of the file/dir
 * @param typeflag corresponding typeflag
 */
static void fill_header(struct tar_header *header, char *path,
                        struct stat *stat, char typeflag)
{
    size_t size = 0;
    if (typeflag == REGTYPE || typeflag == '\0')
    {
        strncpy(header->name, path, sizeof(header->name) - 1);
        size = stat->st_size;
    }
    else
    {
        size_t len = strlen(path);

        if (len < sizeof(header->name) - 1)
        {
            memcpy(header->name, path, len);

            if (path[len - 1] != '/')
            {
                header->name[len] = '/';
            }
        }
    }

    ul_to_oct(header->size, sizeof(header->size), size);

    header->typeflag = typeflag;

    memcpy(header->magic, "ustar", 6);

    ul_to_oct(header->chksum, sizeof(header->chksum) - 1, check_sum(header));
    header->chksum[7] = ' ';
}

/**
 * @brief Add a file to an archive
 * @param ctx Archive context helper struct
 * @param path Path to add
 * @param stat stats of the file
 * @return 0 on success, 1 on failure
 */
static int add_file(struct archive_ctx *ctx, char *path, struct stat *stat)
{
    struct tar_header header = { 0 };
    fill_header(&header, path, stat, REGTYPE);

    FILE *in = fopen(path, "rb");
    if (in == NULL)
    {
        fprintf(stderr, "epitar: unable to add file %s to tarball %s\n", path,
                ctx->archive);
        return 1;
    }
    if (fwrite(&header, 1, BLOCK_SIZE, ctx->out) != BLOCK_SIZE)
    {
        fclose(in);
        fprintf(stderr, "epitar: unable to add file %s to tarball %s\n", path,
                ctx->archive);
        return 1;
    }

    if (ctx->verbose)
    {
        printf("%s\n", path);
    }

    char block[BLOCK_SIZE];
    size_t nread;

    while ((nread = fread(block, 1, BLOCK_SIZE, in)) > 0)
    {
        if (fwrite(block, 1, BLOCK_SIZE, ctx->out) != BLOCK_SIZE)
        {
            return 1;
        }
        if (nread < BLOCK_SIZE)
        {
            memset(block + nread, 0, BLOCK_SIZE - nread);
        }
    }

    fclose(in);
    return 0;
}

/**
 * @brief Search and add a directory to the archive
 * @param ctx Archive context helper struct
 * @param path Path of the dir
 * @param dir Dir stream
 * @return 0 on success, 1 on failure
 */
static int search_dir(struct archive_ctx *ctx, char *path, DIR *dir)
{
    int err = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char *base = strrchr(ctx->archive, '/');
        base = base ? base + 1 : ctx->archive;

        if (strcmp(entry->d_name, base) == 0)
        {
            fprintf(stderr, "epitar: %s is the archive; not dumped\n",
                    ctx->archive);
            continue;
        }

        char tmp[BUF_SIZE];
        size_t len = strlen(path);

        path[len - 1] == '/'
            ? snprintf(tmp, BUF_SIZE, "%s%s", path, entry->d_name)
            : snprintf(tmp, BUF_SIZE, "%s/%s", path, entry->d_name);

        if (add_path(ctx, tmp) != 0)
        {
            err = 1;
        }
    }
    return err;
}

static int add_dir(struct archive_ctx *ctx, char *path, struct stat *stat)
{
    struct tar_header header = { 0 };
    fill_header(&header, path, stat, DIRTYPE);
    if (fwrite(&header, 1, BLOCK_SIZE, ctx->out) != BLOCK_SIZE)
    {
        return 1;
    }
    if (ctx->verbose)
    {
        size_t len = strlen(path);
        if (len > 0 && path[len - 1] == '/')
        {
            printf("%s\n", path);
        }
        else
        {
            printf("%s/\n", path);
        }
    }
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        return 1;
    }

    int err = search_dir(ctx, path, dir);
    closedir(dir);
    return err;
}

int add_path(struct archive_ctx *ctx, char *path)
{
    struct stat stats;
    if (lstat(path, &stats) != 0)
    {
        fprintf(stderr, "epitar: unable to add file %s to tarball %s\n", path,
                ctx->archive);
        return 1;
    }
    if (S_ISREG(stats.st_mode))
    {
        return add_file(ctx, path, &stats);
    }
    if (S_ISDIR(stats.st_mode))
    {
        return add_dir(ctx, path, &stats);
    }

    return 0;
}