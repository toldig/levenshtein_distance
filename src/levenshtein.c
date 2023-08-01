#include "../include/levenshtein.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static uint8_t min (uint8_t num1, uint8_t num2, uint8_t num3);
static uint8_t lev_recursive (char * p_word1,
                              size_t len_word1,
                              char * p_word2,
                              size_t len_word2);


/** @brief return levenshtein distance of 2 given words
 *  @param p_word1 first word
 *  @param p_word2 second word
 **/
uint8_t lev (char * p_word1, char * p_word2)
{
    size_t len_word1 = 0;
    if (NULL != p_word1)
    {
        len_word1 = strlen(p_word1);
    }

    size_t len_word2 = 0;
    if (NULL != p_word2)
    {
        len_word2 = strlen(p_word2);
    }
    return lev_recursive(p_word1, len_word1, p_word2, len_word2);
} /* lev */


/** @brief returns the min value of 3 given values
 *  @param num1 first number
 *  @param num2 second number
 *  @param num3 third number
 **/
static uint8_t min (uint8_t num1, uint8_t num2, uint8_t num3)
{
    if ((num1 >= num2) && (num3 >= num2))
        return num2;
    if ((num2 >= num1) && (num3 >= num1))
        return num1;
    return num3;
} /* min */


/** @brief return levenshtein distance of 2 given words, checks the recursively
 *  @param p_word1 first word
 *  @param len_word1 length of first word
 *  @param p_word2 second word
 *  @param len_word2 length of second word
 **/
static uint8_t lev_recursive (char * p_word1,
                              size_t len_word1,
                              char * p_word2,
                              size_t len_word2)
{
    if (len_word1 == 0 && len_word2 > 0)
    {
        return len_word2;
    }
    if (len_word2 == 0 && len_word1 > 0)
    {
        return len_word1;
    }
    if (len_word2 == 0 && len_word1 == 0)
    {
        return 0;
    }
    if (p_word1[0] == p_word2[0])
    {
        return lev_recursive(p_word1 + 1, len_word1 - 1, p_word2 + 1, len_word2 - 1);
    }

    return 1 + min(lev_recursive(p_word1 + 1, len_word1 - 1, p_word2, len_word2),
                   lev_recursive(p_word1, len_word1, p_word2 + 1, len_word2 - 1),
                   lev_recursive(p_word1 + 1, len_word1 - 1, p_word2 + 1, len_word2 - 1));
} /* lev_recursive */

/* End of source file */