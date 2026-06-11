/**
 * @file resources.h
 * @brief Data structures and allocation helpers used across the pipeline.
 *
 * Defines the Matrix and Parameters structs and declares the factory
 * functions that allocate and initialise them safely.
 */

#ifndef RESOURCES_H
#define RESOURCES_H

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "errorMessages.h"

/**
 * @brief Represents a square matrix backed by a flat heap array.
 */
typedef struct Matrix
{
    FILE*          fileArray; /**< Open file handle for reading or writing. */
    long long int* array;     /**< Row-major flat array of N×N elements.    */
    int            dimension; /**< Side length N of the N×N matrix.         */
} Matrix;

/**
 * @brief Describes the work slice assigned to a single worker thread.
 */
typedef struct Parameters
{
    long long int* array1;     /**< First source (or sole) array.           */
    long long int* array2;     /**< Second source array (sum/multiply).     */
    long long int* array3;     /**< Destination array (sum/multiply).       */
    int            dimension;  /**< Matrix dimension (multiply only).       */
    int            start_pos;  /**< Inclusive start index of this slice.    */
    int            final_pos;  /**< Exclusive end index of this slice.      */
} Parameters;

/** @brief Converts a decimal string to int; exits on parse error. */
int stringToInt(char* str);

/** @brief Opens a file; exits on failure. */
FILE* openFile(char* file_name, char* file_mode);

/** @brief Allocates a zeroed flat array for an N×N matrix. */
long long int* newArray(int dimension);

/** @brief Allocates and initialises a Matrix struct. */
Matrix* newMatrix(char* file_name, char* file_mode, int dimension);

/** @brief Allocates an array of num_threads pthread_t identifiers. */
pthread_t* newThreadIDs(int num_threads);

/** @brief Allocates an array of num_threads Parameters structs. */
Parameters* newParameters(int num_threads);

#endif
