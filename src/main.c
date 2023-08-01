#include "../include/levenshtein.h"
#include <stdio.h>

/** @brief main function, ensure 2 strings are given
 *         return their levenshtein distance
 *  @param argc number of command line arguments
 *  @param argv command line arguments
 **/
int main (int argc, char ** argv)
{
    if (argc != 3)
    {
        printf("Requires 2 strings, %d given\n", argc - 1);
        return -1;
    }
    return lev(argv[1], argv[2]);;
} /* main */

/* End of source file */