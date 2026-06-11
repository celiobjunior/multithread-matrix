/**
 * @file errorMessages.c
 * @brief Runtime guard functions that print to stderr and exit on failure.
 *
 * Every function in this module follows the same pattern: receive a value,
 * test it against an expected "success" state, and terminate the process with
 * a diagnostic message if the check fails.
 *
 * NOTE: This project was recovered from a recorded presentation after the
 * original source files were lost. Comments and documentation are new.
 */

#include "../include/errorMessages.h"

/**
 * @brief Checks whether a thread was created successfully.
 *
 * @param err Return value of pthread_create (0 on success).
 */
void verify_thread_create(int err)
{
    if (err != 0)
    {
        fprintf(stderr, "Thread create failed.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Checks whether a thread was joined successfully.
 *
 * @param err Return value of pthread_join (0 on success).
 */
void verify_thread_join(int err)
{
    if (err != 0)
    {
        fprintf(stderr, "Thread join failed.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Checks whether a dynamic memory allocation succeeded.
 *
 * @param obj Pointer returned by malloc/calloc; NULL indicates failure.
 */
void verify_allocation_memory(void* obj)
{
    if (obj == NULL)
    {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Checks whether a file was opened successfully.
 *
 * @param file      File pointer returned by fopen; NULL indicates failure.
 * @param file_name Name of the file attempted, used in the error message.
 */
void verify_open_file(FILE* file, char* file_name)
{
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open %s file.\n", file_name);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Validates the number of command-line arguments.
 *
 * The program expects exactly 8 arguments (program name + 7 parameters).
 *
 * @param argc Argument count passed to main.
 */
void verify_num_args(int argc)
{
    if (argc != 8)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Validates the thread count.
 *
 * Only 1, 2, and 4 threads are supported. 3 and values greater than 4
 * are rejected because the parallelism model requires powers of 2.
 *
 * @param num_threads Thread count provided by the user.
 */
void verify_num_threads(int num_threads)
{
    if (num_threads <= 0 || num_threads == 3 || num_threads > 4)
    {
        fprintf(stderr, "Invalid number of threads.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Validates the matrix dimension.
 *
 * @param dimension Side length N of the N×N matrix; must be positive.
 */
void verify_num_dimension(int dimension)
{
    if (dimension <= 0)
    {
        fprintf(stderr, "Invalid matrix dimension.\n");
        exit(EXIT_FAILURE);
    }
}
