#ifndef ARCHIVE_UTILS_H
#define ARCHIVE_UTILS_H

#include <stdio.h>

#include "header.h"

/**
 * @brief Archive creation context helper struct
 */
struct archive_ctx
{
    FILE *out; ///< Output archive stream
    char *archive; ///< Archive name
    int verbose; ///< Verbose option
};

/**
 * @brief Add a path to an archive
 * @param ctx Archive context helper struct
 * @param path Path to add
 * @return 0 on success, 1 on failure
 */
int add_path(struct archive_ctx *ctx, char *path);

#endif /* ! ARCHIVE_UTILS_H */