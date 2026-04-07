#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>

/**
 * @brief Size of a single block within the archive
 */
#define BLOCK_SIZE 512

/**
 * @brief Regular file typeflag
 */
#define REGTYPE '0'

/**
 * @brief Directory typeflag
 */
#define DIRTYPE '5'

/**
 * @brief Checksum offset from the start of the header
 */
#define CHKSUM_OFF 148

/**
 * @brief ustar header block (see
 * https://www.mkssoftware.com/docs/man4/tar.4.asp)
 */
struct tar_header
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
};

#endif /* ! HEADER_H */