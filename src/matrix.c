/**
 * @file matrix.c
 * @brief Thread worker functions for matrix I/O and computation.
 *
 * Every function in this module is designed to be passed directly to
 * pthread_create. They all accept a void* argument and return void* so that
 * the same signatures work both as plain calls (single-thread path) and as
 * pthread entry points.
 *
 * NOTE: This project was recovered from a recorded presentation after the
 * original source files were lost. Comments and documentation are new.
 */

#include "../include/matrix.h"

/**
 * @brief Thread function that loads a matrix from its associated file.
 *
 * Reads all space/newline-separated integers from matrix->fileArray into
 * matrix->array in sequential order, then closes the file. The array must
 * already be allocated before this function is called.
 *
 * @param matrix_ref Pointer to a Matrix whose fileArray is open for reading.
 * @return           Always NULL.
 */
void* matrix_transcribe(void* matrix_ref)
{
    Matrix* matrix = (Matrix*) matrix_ref;
    FILE* fileArray = matrix->fileArray;
    long long int* array = matrix->array;
    long long int finput;
    register unsigned int i = 0;

    while (fscanf(fileArray, "%lld", &finput) != EOF)
    {
        array[i] = finput;
        i++;
    }
    fclose(fileArray);
    return NULL;
}

/**
 * @brief Thread function that writes a matrix to its associated file.
 *
 * Writes matrix->array to matrix->fileArray in row-major order, printing
 * each element separated by a space and each row terminated by a newline.
 * Closes the file when done.
 *
 * @param matrix_ref Pointer to a Matrix whose fileArray is open for writing.
 * @return           Always NULL.
 */
void* matrix_write(void* matrix_ref)
{
    Matrix* matrix = (Matrix*) matrix_ref;
    FILE* fileArray = matrix->fileArray;
    long long int* array = matrix->array;
    register unsigned int dimension = matrix->dimension;
    register unsigned int i, j;

    for (i = 0; i < dimension; i++)
    {
        for (j = 0; j < dimension; j++)
        {
            fprintf(fileArray, "%lld ", array[position(i, j, dimension)]);
        }
        fprintf(fileArray, "\n");
    }
    fclose(fileArray);
    return NULL;
}

/**
 * @brief Thread function that performs element-wise addition on array slices.
 *
 * Computes array3[i] = array1[i] + array2[i] for every index i in the
 * half-open range [start_pos, final_pos). Each thread operates on a disjoint
 * slice, so no synchronisation is needed.
 *
 * @param parameters_ref Pointer to a Parameters struct with array1, array2,
 *                       array3, start_pos, and final_pos set.
 * @return               Always NULL.
 */
void* matrix_sum(void* parameters_ref)
{
    Parameters* parameters = (Parameters*) parameters_ref;
    long long int* array1 = parameters->array1;
    long long int* array2 = parameters->array2;
    long long int* array3 = parameters->array3;
    register unsigned int start = parameters->start_pos;
    register unsigned int final = parameters->final_pos;
    register unsigned int i;

    for (i = start; i < final; i++)
    {
        array3[i] = array1[i] + array2[i];
    }
    return NULL;
}

/**
 * @brief Thread function that computes a row-stripe of matrix multiplication.
 *
 * Computes rows [start_pos, final_pos) of the product (array1 × array2) into
 * array3 using the standard triple-loop O(n^3) algorithm. The result array
 * must be zero-initialised (guaranteed by newArray's calloc) so partial
 * accumulations from each thread are correct.
 *
 * @param parameters_ref Pointer to a Parameters struct with array1, array2,
 *                       array3, dimension, start_pos, and final_pos set.
 * @return               Always NULL.
 */
void* matrix_multiplication(void* parameters_ref)
{
    Parameters* parameters = (Parameters*) parameters_ref;
    long long int* array1 = parameters->array1;
    long long int* array2 = parameters->array2;
    long long int* array3 = parameters->array3;
    register unsigned int dimension = parameters->dimension;
    register unsigned int start = parameters->start_pos;
    register unsigned int final = parameters->final_pos;
    register unsigned int i, j, k;

    for (i = start; i < final; i++)
    {
        for (k = 0; k < dimension; k++)
        {
            for (j = 0; j < dimension; j++)
            {
                array3[position(i, j, dimension)] += array1[position(i, k, dimension)] * array2[position(k, j, dimension)];
            }
        }
    }
    return NULL;
}

/**
 * @brief Thread function that sums a slice of a flat matrix array.
 *
 * Accumulates array1[i] for every index i in [start_pos, final_pos) and
 * returns the partial sum as a heap-allocated long long int. The caller
 * (reduce) collects all partial sums via pthread_join and is responsible
 * for freeing the returned pointer.
 *
 * @param parameters_ref Pointer to a Parameters struct with array1,
 *                       start_pos, and final_pos set.
 * @return               Heap-allocated pointer to the partial sum.
 *                       The caller must free this pointer after use.
 */
void* matrix_reduce(void* parameters_ref)
{
    Parameters* parameters = (Parameters*) parameters_ref;
    long long int* array = parameters->array1;
    long long int sum = 0;
    register unsigned int start = parameters->start_pos;
    register unsigned int final = parameters->final_pos;
    register unsigned int i;

    for (i = start; i < final; i++)
    {
        sum += array[i];
    }
    long long int* answer = malloc(sizeof(answer));
    verify_allocation_memory(answer);
    *answer = sum;
    return answer;
}
