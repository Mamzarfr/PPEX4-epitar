#include "parse_opts.h"

#include <stddef.h>

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
