/**
 * @file errorMessages.h
 * @brief Guard functions that validate runtime conditions and exit on failure.
 */

#ifndef ERRORMESSAGES_H
#define ERRORMESSAGES_H

#include <stdio.h>
#include <stdlib.h>

/** @brief Exits if pthread_create returned a non-zero error code. */
void verify_thread_create(int err);

/** @brief Exits if pthread_join returned a non-zero error code. */
void verify_thread_join(int err);

/** @brief Exits if a malloc/calloc pointer is NULL. */
void verify_allocation_memory(void* obj);

/** @brief Exits if a fopen FILE pointer is NULL. */
void verify_open_file(FILE* file, char* file_name);

/** @brief Exits if argc is not exactly 8. */
void verify_num_args(int argc);

/** @brief Exits if num_threads is not 1, 2, or 4. */
void verify_num_threads(int num_threads);

/** @brief Exits if dimension is not positive. */
void verify_num_dimension(int dimension);

#endif
