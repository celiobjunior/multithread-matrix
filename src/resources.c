/**
 * @file resources.c
 * @brief Allocation and initialisation helpers for program resources.
 *
 * Provides safe wrappers around file I/O and heap allocation used throughout
 * the matrix pipeline.
 *
 * NOTE: This project was recovered from a recorded presentation after the
 * original source files were lost. Comments and documentation are new.
 */

#include "../include/resources.h"

/**
 * @brief Converts a string to an int using strtol.
 *
 * Exits with EXIT_FAILURE if the string contains no valid digits or if
 * strtol reports a conversion error via errno.
 *
 * @param str Null-terminated string to convert.
 * @return    The integer value represented by the string.
 */
int stringToInt(char* str)
{
    char* endptr = 0;
    errno = 0;
    long int number = strtol(str, &endptr, 10);

    if (errno != 0)
    {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == str)
    {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }
    return ((int) number);
}

/**
 * @brief Opens a file in the specified mode.
 *
 * Calls verify_open_file internally; exits on failure.
 *
 * @param file_name Path to the file.
 * @param file_mode fopen-compatible mode string (e.g. "r+", "w+").
 * @return          Pointer to the opened FILE stream.
 */
FILE* openFile(char* file_name, char* file_mode)
{
    FILE* file = fopen(file_name, file_mode);
    verify_open_file(file, file_name);
    return file;
}

/**
 * @brief Allocates a zeroed flat array sized for an N×N matrix.
 *
 * Uses calloc so every element starts at zero, which is required for the
 * matrix multiplication accumulation step to be correct.
 *
 * @param dimension Side length N of the square matrix.
 * @return          Pointer to an array of dimension*dimension long long ints.
 */
long long int* newArray(int dimension)
{
    long long int* array = (long long int*) calloc(dimension * dimension, sizeof(long long int));
    verify_allocation_memory(array);
    return array;
}

/**
 * @brief Allocates and initialises a Matrix struct.
 *
 * Opens the backing file, allocates the flat data array, and stores the
 * dimension. The caller is responsible for eventually freeing the struct.
 *
 * @param file_name Path to the matrix data file.
 * @param file_mode fopen-compatible mode string (e.g. "r+", "w+").
 * @param dimension Side length N of the N×N matrix.
 * @return          Pointer to the newly allocated and initialised Matrix.
 */
Matrix* newMatrix(char* file_name, char* file_mode, int dimension)
{
    Matrix* matrix = (Matrix*) malloc(sizeof(Matrix));
    verify_allocation_memory(matrix);
    matrix->fileArray = openFile(file_name, file_mode);
    matrix->array = newArray(dimension);
    matrix->dimension = dimension;
    return matrix;
}

/**
 * @brief Allocates an array of pthread_t thread identifiers.
 *
 * @param num_threads Number of thread IDs to allocate.
 * @return            Pointer to the allocated pthread_t array.
 */
pthread_t* newThreadIDs(int num_threads)
{
    pthread_t* threadIDs = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
    verify_allocation_memory(threadIDs);
    return threadIDs;
}

/**
 * @brief Allocates an array of Parameters structs.
 *
 * Each element describes the work slice (source arrays, start/end indices,
 * dimension) assigned to one worker thread.
 *
 * @param num_threads Number of Parameters entries to allocate.
 * @return            Pointer to the allocated Parameters array.
 */
Parameters* newParameters(int num_threads)
{
    Parameters* parameters = (Parameters*) malloc(num_threads * sizeof(Parameters));
    verify_allocation_memory(parameters);
    return parameters;
}
