#include "tar_utils.h"

#include <stdio.h>
#include <stdlib.h>

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
        {
            return 0;
        }
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

void get_name(struct tar_header *header, char *name, size_t len)
{
    header->prefix[0] != '\0'
        ? snprintf(name, len, "%s/%s", header->prefix, header->name)
        : snprintf(name, len, "%s", header->name);
}

void ul_to_oct(char *buf, size_t len, size_t value)
{
    buf[len - 1] = '\0';

    for (size_t i = len - 1; i > 0; i--)
    {
        buf[i - 1] = '0' + (value & 7);
        value /= 8;
    }
}
