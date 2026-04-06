#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>

#include "header.h"

/**
 * @brief Buffer max size
 */
#define BUF_SIZE 1024

/**
 * @brief -[xcvh] options parsed from argv.
 */
struct opts
{
    int extract; ///< Extraction mode (-x)
    int archive; ///< Archive mode (-c)
    int verbose; ///< Verbose option (-v)
    int help; ///< Help display (-h)
    int start_idx; ///< Index (in argv) of first file to archive, 0 if none
    int count; ///< Number of files to archive
    char *name; ///< Path to the archive name
};

/**
 * @brief Parse -[xcvh] arguments into the opts structure
 * @param argc argc
 * @param argv argv
 * @param opts Output struct
 * @return 0 on success, 1 on error
 */
int parse_opts(int argc, char *argv[], struct opts *opts);

/**
 * @brief Parse an octal string into size_t
 * @param str Octal string from header
 * @param len Length of the string
 * @return Parsed value
 */
size_t oct_to_ul(char *str, size_t len);

/**
 * @brief Check if a block is empty
 * @param block Pointer to a block
 * @return 1 if the block is empty, 0 if not
 */
int empty_block(void *data);

/**
 * @brief Compute the checksum
 * @param header Header
 * @return Checksum value
 */
unsigned int check_sum(struct tar_header *header);

/**
 * @brief Create all directories needed by a path
 * @param path Destination path
 * @return 0 on success, -1 on failure
 */
int create_dirs(char *path);

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
 * @return 0 on success
 */
int extract_dir(struct tar_header *header);

/**
 * @brief Extract a file
 * @param header header
 * @param file Archive stream
 * @param archive Archive name used for error messages
 * @return 0 on success, 3 on failure
 */
int extract_file(struct tar_header *header, FILE *file, char *archive);

/**
 * @brief Build full path
 * @param header header
 * @param name Destination buffer
 * @param len Destination buffer size
 */
void get_name(struct tar_header *header, char *name, size_t len);

#endif /* ! UTILS_H */