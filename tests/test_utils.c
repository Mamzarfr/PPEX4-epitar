#include <criterion/criterion.h>
#include <string.h>

#include "../src/header.h"
#include "../src/utils.h"

TestSuite(utils, .timeout = 5.0);

Test(utils, cv_simple)
{
    char buf[8] = "0000777";
    cr_assert_eq(oct_to_ul(buf, 8), 0777);
}

Test(utils, cv_zero)
{
    char buf[8] = "0000000";
    cr_assert_eq(oct_to_ul(buf, 8), 0);
}

Test(utils, cv_space)
{
    char buf[8] = "741   ";
    cr_assert_eq(oct_to_ul(buf, 8), 0741);
}

Test(utils, cv_null)
{
    char buf[8] = "123";
    cr_assert_eq(oct_to_ul(buf, 8), 0123);
}

Test(utils, empty_block)
{
    char block[BLOCK_SIZE] = { 0 };
    cr_assert_eq(empty_block(block), 1);
}

Test(utils, not_empty)
{
    char block[BLOCK_SIZE] = { 0 };
    block[67] = '1';
    cr_assert_eq(empty_block(block), 0);
}

Test(utils, check_sum)
{
    struct tar_header header = { 0 };
    header.name[0] = 't';
    header.name[1] = 'e';
    header.name[2] = 's';
    header.name[3] = 't';

    for (size_t i = 0; i < 8; i++)
    {
        header.chksum[i] = ' ';
    }

    unsigned int out = check_sum(&header);
    unsigned int ref = 't' + 'e' + 's' + 't' + 8 * ' ';
    cr_assert_eq(out, ref);
}

Test(utils, parse_easy)
{
    struct opts opts = { 0 };
    char *argv[] = { "test", "-x", "test.tar", NULL };
    cr_expect_eq(parse_opts(3, argv, &opts), 0);
    cr_expect_eq(opts.extract, 1);
    cr_expect_str_eq(opts.name, "test.tar");
}

Test(utils, parse_together)
{
    struct opts opts = { 0 };
    char *argv[] = { "test", "-xv", "test.tar", NULL };
    cr_expect_eq(parse_opts(3, argv, &opts), 0);
    cr_expect_eq(opts.extract, 1);
    cr_expect_eq(opts.verbose, 1);
}

Test(utils, parse_sep)
{
    struct opts opts = { 0 };
    char *argv[] = { "test", "-v", "-x", "a.tar", NULL };
    cr_expect_eq(parse_opts(4, argv, &opts), 0);
    cr_expect_eq(opts.extract, 1);
    cr_expect_eq(opts.verbose, 1);
}

Test(utils, parse_help)
{
    struct opts opts = { 0 };
    char *argv[] = { "test", "-h", NULL };
    cr_expect_eq(parse_opts(2, argv, &opts), 0);
    cr_expect_eq(opts.help, 1);
}

Test(utils, parse_help_tricky)
{
    struct opts opts = { 0 };
    char *argv[] = { "test", "-z", "-h", NULL };
    cr_expect_eq(parse_opts(3, argv, &opts), 0);
    cr_expect_eq(opts.help, 1);
}

Test(utils, parse_invalid)
{
    struct opts opts = { 0 };
    char *argv[] = { "test", "-test", "test.tar", NULL };
    cr_expect_eq(parse_opts(3, argv, &opts), 1);
}

Test(utils, parse_full)
{
    struct opts opts = { 0 };
    char *argv[] = {
        "test", "-xcvh", "test.tar", "test1.txt", "test2.txt", NULL
    };
    cr_expect_eq(parse_opts(5, argv, &opts), 0);
    cr_expect_eq(opts.extract, 1);
    cr_expect_eq(opts.archive, 1);
    cr_expect_eq(opts.verbose, 1);
    cr_expect_eq(opts.help, 1);
    cr_expect_eq(opts.start_idx, 3);
    cr_expect_eq(opts.count, 2);
    cr_expect_str_eq(opts.name, "test.tar");
}

Test(utils, get_name)
{
    struct tar_header header = { 0 };
    char buf[1024];
    strcpy(header.name, "test.txt");

    get_name(&header, buf, sizeof(buf));
    cr_expect_str_eq(buf, "test.txt");
}

Test(utils, get_name_dir)
{
    struct tar_header header = { 0 };
    char buf[1024];
    strcpy(header.name, "test.txt");
    strcpy(header.prefix, "dir");

    get_name(&header, buf, sizeof(buf));
    cr_expect_str_eq(buf, "dir/test.txt");
}