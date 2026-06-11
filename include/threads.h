/**
 * @file threads.h
 * @brief Pipeline stage orchestrators for the multithreaded matrix workflow.
 *
 * Each function manages thread lifecycle (create / join / free) for one
 * stage of the pipeline. When num_threads == 1 the work is done inline
 * without creating any POSIX threads.
 */

#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include "errorMessages.h"
#include "matrix.h"
#include "resources.h"

/** @brief Loads A and B from their files, concurrently when possible. */
void transcribe_A_and_B(Matrix* matrix_A, Matrix* matrix_B, int num_threads);

/** @brief Computes D = A + B in parallel across num_threads threads. */
void sum(Matrix* matrix_A, Matrix* matrix_B, Matrix* matrix_D, int dimension, int num_threads);

/** @brief Writes D to its file and loads C, concurrently when possible. */
void write_D_transcribe_C(Matrix* matrix_C, Matrix* matrix_D, int num_threads);

/** @brief Computes E = C × D in parallel across num_threads threads. */
void multiply(Matrix* matrix_C, Matrix* matrix_D, Matrix* matrix_E, int dimension, int num_threads);

/** @brief Writes E to its file and returns the sum of all its elements. */
long long int reduce(Matrix* matrix_E, int dimension, int num_threads);

#endif
