#define _POSIX_C_SOURCE 200809L

#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "header.h"

static int set_opts(char *arg, struct opts *opts)
{
    for (int i = 1; arg[i] != '\0'; i++)
    {
        switch (arg[i])
        {
        case 'x':
            opts->extract = 1;
            break;
        case 'c':
            opts->archive = 1;
            break;
        case 'v':
            opts->verbose = 1;
            break;
        case 'h':
            opts->help = 1;
            break;
        default:
            return 1;
        }
    }
    return 0;
}

int parse_opts(int argc, char *argv[], struct opts *opts)
{
    int err = 0;

    for (int i = 1; i < argc; i++)
    {
        if (opts->name == NULL && argv[i][0] == '-' && argv[i][1] != '\0')
        {
            err = set_opts(argv[i], opts);
        }
        else if (opts->name == NULL)
        {
            opts->name = argv[i];
        }
        else
        {
            if (opts->start_idx == 0)
            {
                opts->start_idx = i;
            }
            opts->count = argc - i;
            break;
        }
    }

    if (opts->help)
    {
        return 0;
    }
    return err;
}

size_t oct_to_ul(char *str, size_t len)
{
    size_t res = 0;
    for (size_t i = 0; i < len && str[i] != '\0'; i++)
    {
        if (str[i] >= '0' && str[i] <= '7')
        {
            res = res * 8 + (str[i] - '0');
        }
    }

    return res;
}

int empty_block(void *data)
{
    char *block = data;
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (block[i] != 0)
            return 0;
    }
    return 1;
}

unsigned int check_sum(struct tar_header *header)
{
    void *void_ptr = header;
    unsigned char *ptr = void_ptr;
    unsigned int sum = 0;

    for (size_t i = 0; i < BLOCK_SIZE; i++)
    {
        if (i >= CHKSUM_OFF && i < CHKSUM_OFF + sizeof(header->chksum))
        {
            sum += ' ';
        }
        else
        {
            sum += ptr[i];
        }
    }

    return sum;
}

int create_dirs(char *path)
{
    char *buf = strdup(path);
    if (buf == NULL)
    {
        return -1;
    }

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
    return 0;
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

int extract_dir(struct tar_header *header)
{
    char buf[BUF_SIZE];
    get_name(header, buf, sizeof(buf));

    create_dirs(buf);
    mkdir(buf, 0755);

    return 0;
}

int extract_file(struct tar_header *header, FILE *file, char *archive)
{
    char buf[BUF_SIZE];

    get_name(header, buf, sizeof(buf));
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
            return 3;
        }

        fwrite(block, 1, nread, os);
        size -= nread;
    }

    fclose(os);
    chmod(buf, oct_to_ul(header->mode, 8));

    return 0;
}

void get_name(struct tar_header *header, char *name, size_t len)
{
    if (header->prefix[0] != '\0')
    {
        snprintf(name, len, "%s/%s", header->prefix, header->name);
    }
    else
    {
        snprintf(name, len, "%s", header->name);
    }
}