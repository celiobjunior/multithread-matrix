/**
 * @file threads.c
 * @brief High-level orchestration of the multithreaded matrix pipeline.
 *
 * Each function in this module coordinates one stage of the pipeline. When
 * num_threads == 1 the work is performed sequentially in the calling thread;
 * otherwise POSIX threads are spawned, their workloads are set up in a
 * Parameters array, and the threads are joined before the function returns.
 *
 * NOTE: This project was recovered from a recorded presentation after the
 * original source files were lost. Comments and documentation are new.
 */

#include "../include/threads.h"

/**
 * @brief Loads matrices A and B from their files, optionally in parallel.
 *
 * With num_threads == 1 the reads execute sequentially. With more threads,
 * two dedicated threads read each matrix concurrently, overlapping file I/O.
 *
 * @param matrix_A    Pointer to the Matrix struct for A (open for reading).
 * @param matrix_B    Pointer to the Matrix struct for B (open for reading).
 * @param num_threads Number of worker threads (1, 2, or 4).
 */
void transcribe_A_and_B(Matrix* matrix_A, Matrix* matrix_B, int num_threads)
{
    if (num_threads == 1)
    {
        matrix_transcribe((void*) matrix_A);
        matrix_transcribe((void*) matrix_B);
    }
    else
    {
        pthread_t* thread_ids = newThreadIDs(num_threads);
        register int i;
        int err;

        err = pthread_create(&thread_ids[0], NULL, matrix_transcribe, (void*) matrix_A);
        verify_thread_create(err);

        err = pthread_create(&thread_ids[1], NULL, matrix_transcribe, (void*) matrix_B);
        verify_thread_create(err);

        for (i = 0; i < 2; i++)
        {
            err = pthread_join(thread_ids[i], NULL);
            verify_thread_join(err);
        }
        free(thread_ids);
    }
}

/**
 * @brief Computes D = A + B using num_threads worker threads.
 *
 * Divides the flat element array into num_threads equal slices and assigns
 * each slice to a separate thread running matrix_sum. Frees matrix_A and
 * matrix_B after all threads complete.
 *
 * @param matrix_A    Pointer to the first operand matrix.
 * @param matrix_B    Pointer to the second operand matrix.
 * @param matrix_D    Pointer to the result matrix (pre-allocated, zeroed).
 * @param dimension   Side length N of the N×N matrices.
 * @param num_threads Number of worker threads (1, 2, or 4).
 */
void sum(Matrix* matrix_A, Matrix* matrix_B, Matrix* matrix_D, int dimension, int num_threads)
{
    Parameters* parameters = newParameters(num_threads);
    register int num_elements = (dimension * dimension) / num_threads;
    register int i;

    for (i = 0; i < num_threads; i++)
    {
        parameters[i].array1 = matrix_A->array;
        parameters[i].array2 = matrix_B->array;
        parameters[i].array3 = matrix_D->array;
        parameters[i].start_pos = num_elements * i;
        parameters[i].final_pos = (num_elements * (i + 1));
    }

    if (num_threads == 1)
    {
        matrix_sum((void*) &parameters[0]);
        free(parameters);
    }
    else
    {
        pthread_t* thread_ids = newThreadIDs(num_threads);
        int err;

        for (i = 0; i < num_threads; i++)
        {
            err = pthread_create(&thread_ids[i], NULL, matrix_sum, (void*) &parameters[i]);
            verify_thread_create(err);
        }

        for (i = 0; i < num_threads; i++)
        {
            err = pthread_join(thread_ids[i], NULL);
            verify_thread_join(err);
        }
        free(parameters);
        free(thread_ids);
    }
    free(matrix_A);
    free(matrix_B);
}

/**
 * @brief Writes matrix D to its file and loads matrix C, optionally in parallel.
 *
 * With num_threads == 1 the write and read execute sequentially. With more
 * threads, two dedicated threads handle each task concurrently, overlapping
 * disk I/O with the file read.
 *
 * @param matrix_C    Pointer to the Matrix struct for C (open for reading).
 * @param matrix_D    Pointer to the Matrix struct for D (open for writing).
 * @param num_threads Number of worker threads (1, 2, or 4).
 */
void write_D_transcribe_C(Matrix* matrix_C, Matrix* matrix_D, int num_threads)
{
    if (num_threads == 1)
    {
        matrix_write((void*) matrix_D);
        matrix_transcribe((void*) matrix_C);
    }
    else
    {
        pthread_t* thread_ids = newThreadIDs(num_threads);
        register int i;
        int err;

        err = pthread_create(&thread_ids[0], NULL, matrix_write, (void*) matrix_D);
        verify_thread_create(err);

        err = pthread_create(&thread_ids[1], NULL, matrix_transcribe, (void*) matrix_C);
        verify_thread_create(err);

        for (i = 0; i < 2; i++)
        {
            err = pthread_join(thread_ids[i], NULL);
            verify_thread_join(err);
        }
        free(thread_ids);
    }
}

/**
 * @brief Computes E = C × D using num_threads worker threads.
 *
 * Divides the row range [0, dimension) into num_threads equal slices and
 * assigns each slice to a separate thread running matrix_multiplication.
 * Frees matrix_C and matrix_D after all threads complete.
 *
 * @param matrix_C    Pointer to the left operand matrix.
 * @param matrix_D    Pointer to the right operand matrix.
 * @param matrix_E    Pointer to the result matrix (pre-allocated, zeroed).
 * @param dimension   Side length N of the N×N matrices.
 * @param num_threads Number of worker threads (1, 2, or 4).
 */
void multiply(Matrix* matrix_C, Matrix* matrix_D, Matrix* matrix_E, int dimension, int num_threads)
{
    Parameters* parameters = newParameters(num_threads);
    register int num_lines = dimension / num_threads;
    register int i;

    for (i = 0; i < num_threads; i++)
    {
        parameters[i].array1 = matrix_C->array;
        parameters[i].array2 = matrix_D->array;
        parameters[i].array3 = matrix_E->array;
        parameters[i].dimension = dimension;
        parameters[i].start_pos = num_lines * i;
        parameters[i].final_pos = (num_lines * (i + 1));
    }

    if (num_threads == 1)
    {
        matrix_multiplication((void*) &parameters[0]);
        free(parameters);
    }
    else
    {
        pthread_t* thread_ids = newThreadIDs(num_threads);
        int err;

        for (i = 0; i < num_threads; i++)
        {
            err = pthread_create(&thread_ids[i], NULL, matrix_multiplication, (void*) &parameters[i]);
            verify_thread_create(err);
        }

        for (i = 0; i < num_threads; i++)
        {
            err = pthread_join(thread_ids[i], NULL);
            verify_thread_join(err);
        }
        free(parameters);
        free(thread_ids);
    }
    free(matrix_C);
    free(matrix_D);
}

/**
 * @brief Writes matrix E to its file and computes the global element sum.
 *
 * Spawns num_threads reduction threads that each accumulate a disjoint slice
 * of matrix_E->array, plus one additional thread that concurrently writes E
 * to disk. After joining all threads the partial sums are accumulated into
 * a single result. Frees matrix_E after completion.
 *
 * @param matrix_E    Pointer to the result matrix to be reduced and written.
 * @param dimension   Side length N of the N×N matrix.
 * @param num_threads Number of worker threads (1, 2, or 4).
 * @return            Sum of all N×N elements in matrix_E.
 */
long long int reduce(Matrix* matrix_E, int dimension, int num_threads)
{
    Parameters* parameters = newParameters(num_threads + 1);
    register int num_elements = (dimension * dimension) / num_threads;
    register int i;
    long long int sum = 0;

    for (i = 0; i < num_threads; i++)
    {
        parameters[i].array1 = matrix_E->array;
        parameters[i].start_pos = num_elements * i;
        parameters[i].final_pos = (num_elements * (i + 1));
    }

    if (num_threads == 1)
    {
        matrix_write((void*) matrix_E);
        sum = *(long long int*) matrix_reduce((void*) &parameters[0]);
    }
    else
    {
        pthread_t* thread_ids = newThreadIDs(num_threads + 1);
        long long int* partialSum = NULL;
        int err;

        err = pthread_create(&thread_ids[num_threads], NULL, matrix_write, (void*) matrix_E);
        verify_thread_create(err);

        for (i = 0; i < num_threads; i++)
        {
            err = pthread_create(&thread_ids[i], NULL, matrix_reduce, (void*) &parameters[i]);
            verify_thread_create(err);
        }

        err = pthread_join(thread_ids[num_threads], NULL);
        verify_thread_join(err);

        for (i = 0; i < num_threads; i++)
        {
            err = pthread_join(thread_ids[i], (void**) &partialSum);
            verify_thread_join(err);

            sum += *(long long int*) partialSum;
        }
        free(parameters);
        free(thread_ids);
    }
    free(matrix_E);
    return sum;
}
