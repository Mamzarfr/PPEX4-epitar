#define _POSIX_C_SOURCE 200809L

#include "archive.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "archive_utils.h"

int create_archive(char *archive, char **files, int count, int verbose)
{
    if (count == 0)
    {
        fprintf(stderr,
                "epitar: cowardly refusing to create an empty archive\n");
        return 1;
    }

    FILE *out = fopen(archive, "wb");
    if (out == NULL)
    {
        return 3;
    }

    struct archive_ctx ctx = { .out = out,
                               .archive = archive,
                               .verbose = verbose };

    int err = 0;
    for (int i = 0; i < count; i++)
    {
        if (add_path(&ctx, files[i]) != 0)
        {
            err = 3;
        }
    }

    char block[BLOCK_SIZE] = { 0 };

    if (fwrite(block, 1, BLOCK_SIZE, out) != BLOCK_SIZE
        || fwrite(block, 1, BLOCK_SIZE, out) != BLOCK_SIZE)
    {
        err = 3;
    }

    fclose(out);
    return err;
}
