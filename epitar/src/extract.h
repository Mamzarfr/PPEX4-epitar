#ifndef EXTRACT_H
#define EXTRACT_H

/**
 * @brief Extract a tar archive
 * @param archive Path to the archive
 * @param verbose Verbose option
 * @return 0 on success, error number on failure
 */
int extract_archive(char *archive, int verbose);

#endif /* ! EXTRACT_H */