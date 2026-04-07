#define _POSIX_C_SOURCE 200809L

#include "extract_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tar_utils.h"

void create_dirs(char *path)
{
    char *buf = strdup(path);

    for (char *c = buf; *c != '\0'; c++)
    {
        if (*c == '/')
        {
            *c = '\0';
            mkdir(buf, 0755);
            *c = '/';
        }
    }

    free(buf);
}

int check_header(struct tar_header *header, char *archive)
{
    if (strncmp(header->magic, "ustar", 5) != 0)
    {
        fprintf(stderr, "epitar: error extracting tarball %s\n", archive);
        return 3;
    }

    if (oct_to_ul(header->chksum, 8) != check_sum(header))
    {
        fprintf(stderr, "epitar: bad checksum\n");
        return 2;
    }
    return 0;
}

void extract_dir(struct tar_header *header)
{
    char buf[BUF_SIZE];

    get_name(header, buf, BUF_SIZE);
    create_dirs(buf);
    mkdir(buf, 0755);
}

int extract_file(struct tar_header *header, FILE *file, char *archive)
{
    char buf[BUF_SIZE];

    get_name(header, buf, BUF_SIZE);
    create_dirs(buf);

    FILE *os = fopen(buf, "wb");
    if (os == NULL)
    {
        fprintf(stderr, "epitar: error extracting tarball %s\n", archive);
        return 3;
    }

    char block[BLOCK_SIZE];
    size_t size = oct_to_ul(header->size, 12);
    while (size > 0)
    {
        size_t nread = size < BLOCK_SIZE ? size : BLOCK_SIZE;

        if (fread(block, 1, BLOCK_SIZE, file) != BLOCK_SIZE)
        {
            fclose(os);
            fprintf(stderr, "epitar: error extracting tarball %s\n", archive);
            return 3;
        }

        fwrite(block, 1, nread, os);
        size -= nread;
    }

    fclose(os);
    return 0;
}