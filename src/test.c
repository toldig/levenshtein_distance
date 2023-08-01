#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <time.h>
#include "../include/levenshtein.h"
#include <stdbool.h>

/**
 * @brief  Do not go crazy with the test cases
 *         Letter must be unique after change 
 *         So if MAXLEN or MAXDIFF are changed to a very big number,
 *         it might not work
 **/
#define MINLEN 5
#define MAXLEN 10
#define MINDIFF 2
#define MAXDIFF 4
#define LOOP_COUNTER 500
#define NULL_TERMINATOR 1
#define ALPHA_MIN 97
#define ALPHA_MAX 122


static bool generate_case (char ** p_word1, char ** p_word2, uint8_t distance);
static void generate_word (char * p_word1, uint8_t len);
static bool change_word (char * p_word1,
                         uint8_t len,
                         uint8_t remove,
                         uint8_t change);
static uint8_t generate_random (uint8_t min, uint8_t max, bool include_max);
static char generate_letter (void);
static uint8_t * generate_unique_randoms(uint8_t len,
                                         uint8_t min,
                                         uint8_t max,
                                         bool include);
static bool generate_unique_randoms_with_exclude (uint8_t * p_indexes,
                                                  uint8_t current,
                                                  char * p_word,
                                                  char * p_excludes,
                                                  uint8_t * p_exclude_index,
                                                  uint8_t min,
                                                  uint8_t max,
                                                  bool include);


/** @brief test if invalid inputs work **/
void test_invalid_inputs (void)
{
    for (int index = 0; index < LOOP_COUNTER; index++)
    {
        char * p_word1 = NULL;
        char * p_word2 = NULL;
        if (generate_case(&p_word1, &p_word2, 0))
        {
            uint8_t len = strlen(p_word1);
            CU_ASSERT_TRUE(len == lev(NULL, p_word2));
            CU_ASSERT_TRUE(len == lev(p_word1, NULL));
            CU_ASSERT_TRUE(0 == lev(NULL, NULL));
        }
        else
        {
            index--;
        }
        free(p_word1);
        p_word1 = NULL;
        free(p_word2);
        p_word2 = NULL;
    }
} /* test_invalid_inputs */


/** @brief test if equal inputs work **/
void test_equal_inputs (void)
{
    for (int index = 0; index < LOOP_COUNTER; index++)
    {
        char * p_word1 = NULL;
        char * p_word2 = NULL;
        if (generate_case(&p_word1, &p_word2, 0))
        {
            CU_ASSERT_TRUE(0 == lev(p_word1, p_word2));
            CU_ASSERT_TRUE(0 == lev(p_word2, p_word1));
        }
        else
        {
            index--;
        }
        free(p_word1);
        p_word1 = NULL;
        free(p_word2);
        p_word2 = NULL;
    }
} /* test_equal_inputs */


/** @brief test if different inputs work **/
void test_different_inputs (void)
{
    for (int index = 0; index < LOOP_COUNTER; index++)
    {
        char * p_word1 = NULL;
        char * p_word2 = NULL;
        uint8_t diff = generate_random(MINDIFF, MAXDIFF, true);
        
        if (generate_case(&p_word1, &p_word2, diff))
        {
            
            bool res = diff == lev(p_word1, p_word2);
            CU_ASSERT_TRUE(res);
            res = diff == lev(p_word2, p_word1);
            CU_ASSERT_TRUE(res);
        }
        else
        {
            index--;
        }
        free(p_word1);
        p_word1 = NULL;
        free(p_word2);
        p_word2 = NULL;
    }
} /* test_different_inputs */


/** @brief generates a random number between the given ranges
 *  @param min min range
 *  @param max max range
 *  @param include_max is max included in range?
 **/
static uint8_t generate_random (uint8_t min, uint8_t max, bool include_max)
{
    if (include_max)
        return (rand() % (max - min + 1)) + min;
    return (rand() % (max - min)) + min;
} /* generate_random */


/** @brief generates a lower case letter **/
static char generate_letter (void)
{
    return generate_random(ALPHA_MIN, ALPHA_MAX, true);
} /* generate_letter */


/** @brief generates unique random index, makes sure letter new letter is not
 *         in the exclude list and also not in the word
 *  @param p_indexes unique numbers
 *  @param current current index of unique number we need to generate
 *  @param p_word word, exlude all these letters
 *  @param p_excludes exclude list, also exclude all these letters
 *  @param min min range of new index
 *  @param max max range of new index
 *  @param include_max is max included in range?
 **/
static bool generate_unique_randoms_with_exclude (uint8_t * p_indexes,
                                                  uint8_t current,
                                                  char * p_word,
                                                  char * p_excludes,
                                                  uint8_t * p_exclude_index,
                                                  uint8_t min,
                                                  uint8_t max,
                                                  bool include)
{
    if (NULL == p_indexes || p_word == NULL || p_excludes == NULL)
    {
        return false;
    }
    bool run = true;
    while (run)
    {
        run = false;
        uint8_t random_index = generate_random(min, max, include);
        // Check if new_random is in p_indexes
        uint8_t index = 0;
        for (; index < current; index++)
        {
            if (p_indexes[index] == random_index)
            {
                run = true;
                break;
            }
        }
        // Check if letter at new_random is in p_exludes
        index = 0;
        if (!run && strchr(p_excludes, p_word[random_index]))
        {
            run = true;
        }
        index = 0;
        if (!run)
        {
            // Generate a new letter that is not in the word already
            char letter = generate_letter();
            while ( NULL != strchr(p_word, letter))
            {
                letter = generate_letter();
            }
            // Save index, save old in exclude, overwrite word with new letter
            p_indexes[current] = random_index;
            p_excludes[*p_exclude_index++] = p_word[random_index];
            p_word[random_index] = letter;
        }
    }
    return true;
} /* generate_unique_randoms_with_exclude */


/** @brief generates unique random numbers in the given range
 *  @param len number of unique numbers
 *  @param min min range of new index
 *  @param max max range of new index
 *  @param include_max is max included in range?
 **/
static uint8_t * generate_unique_randoms(uint8_t len,
                                         uint8_t min,
                                         uint8_t max,
                                         bool include)
{
    uint8_t * p_indexes = calloc(len, sizeof(uint8_t));
    if (NULL == p_indexes)
    {
        return NULL;
    }
    // Initialize with UINT8_MAX
    for (uint8_t i = 0; i < len; i++)
    {
        p_indexes[i] = UINT8_MAX;
    }
    for (uint8_t i = 0; i < len; i++)
    {
        bool run = true;
        while (run)
        {
            run = false;
            uint8_t new_index = generate_random(min, max, include);
            for (uint8_t tmp_i = 0; tmp_i < i; tmp_i++)
            {
                if (p_indexes[tmp_i] == new_index)
                {
                    run = true;
                    break;
                }
            }
            if (!run)
            {
                p_indexes[i] = new_index;
            }
        }
    }
    return p_indexes;
} /* generate_unique_randoms */


/** @brief changes the word by removing or changing some letters
 *  @param p_word word to change
 *  @param len length of the word
 *  @param remove number of characters to remove
 *  @param change number of characters to change
 **/
static bool change_word (char * p_word,
                         uint8_t len,
                         uint8_t remove,
                         uint8_t change)
{
    if (NULL == p_word)
    {
        return false;
    }
    char excludes[MAXDIFF + NULL_TERMINATOR];
    memset(excludes, 0, sizeof(excludes));
    uint8_t exclude_index = 0;
    // Remove characters at random indexes
    if (remove)
    {
        uint8_t new_len = 0;
        uint8_t * p_remove_indexes = generate_unique_randoms(remove, 0, len, false);
        if (NULL == p_remove_indexes)
        {
            return false;
        }
        for (uint8_t index = 0; index < len; index++)
        {
            bool can_copy = true;
            for (uint8_t remove_i = 0; remove_i < remove; remove_i++)
            {
                if (index == p_remove_indexes[remove_i])
                {
                    excludes[exclude_index++] = p_word[index];
                    can_copy = false;
                    break;
                }
            }
            if (can_copy)
            {
                p_word[new_len++] = p_word[index];
            }
        }
        free(p_remove_indexes);
        p_remove_indexes = NULL;
        len = new_len;
        p_word[len] = 0;
    }
    // Change characters at random indexes
    if (change)
    {
        uint8_t * p_change_indexes = calloc(change, sizeof(uint8_t));
        if (NULL == p_change_indexes)
        {
            return false;
        }
        // Initialize with UINT8_MAX
        for (uint8_t i = 0; i < change; i++)
        {
            p_change_indexes[i] = UINT8_MAX;
        }
        for (uint8_t change_i = 0; change_i < change; change_i++)
        {
            if (generate_unique_randoms_with_exclude (p_change_indexes,
                                                  change_i,
                                                  p_word,
                                                  excludes,
                                                  &exclude_index,
                                                  0,
                                                  len,
                                                  false))
                {
                    free(p_change_indexes);
                    p_change_indexes = NULL;
                    return false;
                }
            
            
        }
        free(p_change_indexes);
        p_change_indexes = NULL;
    }
    return true;
} /* change_word */


/** @brief generate test case
 *         two words with the provided distance
 *  @param p_word1 first word, will be allocated, caller must free it
 *  @param p_word2 second word, will be allocated, caller must free it
 *  @param distance given distance
 **/
static bool generate_case (char ** p_word1, char ** p_word2, uint8_t distance)
{
    // Generate random word
    uint8_t minlen = MINLEN;
    if (distance > minlen)
    {
        minlen = distance;
    }
    int len = generate_random(minlen, MAXLEN, true);
    *p_word1 = calloc(len + NULL_TERMINATOR, sizeof(char));
    if (NULL == *p_word1)
    {
        return false;
    }
    generate_word(*p_word1, len);
    // Make a copy of the word
    *p_word2 = calloc(len + NULL_TERMINATOR, sizeof(char));
    if (NULL == *p_word1)
    {   
        free(*p_word1);
        *p_word1 = NULL;
        return false;
    }
    strncpy(*p_word2, *p_word1, len);
    if (0 != distance)
    {
        uint8_t remove = generate_random(0, distance, true);
        uint8_t change = distance - remove;
        if (!change_word(*p_word2, len, remove, change))
        {
            return false;
        }
    }
    return true;
} /* generate_case */


/** @brief generate_case generate a word 
 *  @param p_word pointer to the word
 *  @param len length of the word
 * **/
static void generate_word (char * p_word, uint8_t len)
{
    for (int index = 0; index < len; index++)
    {
        p_word[index] =  generate_letter();
    }
    p_word[len] = '\0';
    return;
} /* generate_word */


/** @brief main function of the tester
 *  @param argc number of command line arguments
 *  @param argv command line arguments
 **/
int main(int argc, char* argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    CU_pSuite cache_test_suite = CU_add_suite("Test Levemshtein Distance", NULL, NULL);
 
    if (NULL == cache_test_suite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    unsigned int seed = time(0);
    // seed = 7;
    srand(seed);

    if ( ( NULL == CU_add_test(cache_test_suite,
                               "test of invalid inputs",
                               test_invalid_inputs) ) || 
         ( NULL == CU_add_test(cache_test_suite,
                               "test of equal inputs",
                               test_equal_inputs) ) ||
         ( NULL == CU_add_test(cache_test_suite,
                               "test of different inputs",
                               test_different_inputs)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_automated_enable_junit_xml(CU_TRUE);
    CU_automated_run_tests();

    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
} /* main */

/* End of source file */