#ifndef PARSE_OPTS_H
#define PARSE_OPTS_H

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

#endif /* ! PARSE_OPTS_H */
