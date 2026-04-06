#define _POSIX_C_SOURCE 200809L

#include "extract.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "header.h"
#include "utils.h"

int extract_archive(char *archive, int verbose)
{
    FILE *file = fopen(archive, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "epitar: error extracting tarball %s\n", archive);
        return 3;
    }

    struct tar_header header;
    while (fread(&header, 1, BLOCK_SIZE, file) == BLOCK_SIZE)
    {
        if (empty_block(&header))
        {
            struct tar_header next;
            if (fread(&next, 1, BLOCK_SIZE, file) != BLOCK_SIZE
                || empty_block(&next))
            {
                break;
            }
            continue;
        }

        int err = check_header(&header, archive);
        if (err != 0)
        {
            fclose(file);
            return err;
        }

        if (verbose)
        {
            char name[BUF_SIZE];
            get_name(&header, name, BUF_SIZE);
            printf("%s\n", name);
        }

        if (header.typeflag == DIRTYPE)
        {
            extract_dir(&header);
        }
        else if (header.typeflag == REGTYPE || header.typeflag == '\0')
        {
            int ret = extract_file(&header, file, archive);
            if (ret != 0)
            {
                fclose(file);
                return ret;
            }
        }
    }

    fclose(file);
    return 0;
}