#define _POSIX_C_SOURCE 200809L

#include "extract.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "extract_utils.h"
#include "header.h"
#include "tar_utils.h"

int extract_archive(char *archive, int verbose)
{
    char *base = strrchr(archive, '/');
    base = base ? base + 1 : archive;

    FILE *file = fopen(archive, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "epitar: error extracting tarball %s\n", base);
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

        int err = check_header(&header, base);
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
            if (extract_file(&header, file, base) != 0)
            {
                fclose(file);
                return 3;
            }
        }
    }

    fclose(file);
    return 0;
}
