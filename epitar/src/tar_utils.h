#ifndef TAR_UTILS_H
#define TAR_UTILS_H

#include <stddef.h>

#include "header.h"

/**
 * @brief Buffer max size
 */
#define BUF_SIZE 256

/**
 * @brief Parse an octal string into size_t
 * @param str Octal string from header
 * @param len Length of the string
 * @return Parsed value
 */
size_t oct_to_ul(char *str, size_t len);

/**
 * @brief Convert a size_t into an octal string and put it in a buffer
 * @param buf Output buffer
 * @param len Length of the output buffer
 * @param value Value to convert
 */
void ul_to_oct(char *buf, size_t len, size_t value);

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
 * @brief Build full path
 * @param header header
 * @param name Destination buffer
 * @param len Destination buffer size
 */
void get_name(struct tar_header *header, char *name, size_t len);

#endif /* ! TAR_UTILS_H */
