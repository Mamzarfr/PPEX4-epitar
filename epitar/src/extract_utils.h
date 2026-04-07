#ifndef EXTRACT_UTILS_H
#define EXTRACT_UTILS_H

#include <stdio.h>

#include "header.h"

/**
 * @brief Create all directories needed by a path
 * @param path Destination path
 */
void create_dirs(char *path);

/**
 * @brief Make sure a header is valid
 * @param header Header to check
 * @param archive Archive name used for error messages
 * @return 0 if valid, error code otherwise
 */
int check_header(struct tar_header *header, char *archive);

/**
 * @brief Extract a directory
 * @param header header
 */
void extract_dir(struct tar_header *header);

/**
 * @brief Extract a file
 * @param header header
 * @param file Archive stream
 * @param archive Archive name used for error messages
 * @return 0 on success, 3 on failure
 */
int extract_file(struct tar_header *header, FILE *file, char *archive);

#endif /* ! EXTRACT_UTILS_H */