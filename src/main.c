#include <stdio.h>

#include "archive.h"
#include "extract.h"
#include "parse_opts.h"

/**
 * @brief Print usage help
 */
static void print_help(void)
{
    printf("Usage: epitar -[xcvh] <file.tar> [<files>]\n");
}

/**
 * @brief Print error message for invalid options
 */
static void print_err(void)
{
    fprintf(stderr,
            "epitar: invalid or missing option\nTry './epitar -h' for more "
            "information.\n");
}

int main(int argc, char *argv[])
{
    struct opts opts = { 0 };

    if (parse_opts(argc, argv, &opts) != 0)
    {
        print_err();
        return 1;
    }
    if (opts.help)
    {
        print_help();
        return 0;
    }
    if (opts.extract == opts.archive)
    {
        print_err();
        return 1;
    }
    if (opts.name == NULL)
    {
        print_err();
        return 1;
    }
    if (opts.extract)
    {
        return extract_archive(opts.name, opts.verbose);
    }
    return create_archive(opts.name, argv + opts.start_idx, opts.count,
                          opts.verbose);
}
