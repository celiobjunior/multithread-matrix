/**
 * @file matrix.h
 * @brief Thread worker functions for matrix file I/O and arithmetic.
 *
 * All functions follow the pthread_create signature (void* → void*) so they
 * can be used both directly (single-thread path) and as thread entry points.
 */

#ifndef MATRIX_H
#define MATRIX_H

#include "resources.h"

/**
 * @brief Converts (row, col) indices to a flat row-major array offset.
 *
 * @param i         Row index.
 * @param j         Column index.
 * @param dimension Side length of the square matrix.
 */
#define position(i, j, dimension) ((i) * (dimension) + (j))

/** @brief Reads a matrix from its file into its backing array. */
void* matrix_transcribe(void* matrix_ref);

/** @brief Writes a matrix's backing array to its file. */
void* matrix_write(void* matrix_ref);

/** @brief Element-wise addition of two array slices into a third. */
void* matrix_sum(void* parameters_ref);

/** @brief Row-stripe matrix multiplication (C = A × B). */
void* matrix_multiplication(void* parameters_ref);

/**
 * @brief Partial sum of an array slice; returns a heap-allocated result.
 *
 * @note The caller must free the returned pointer.
 */
void* matrix_reduce(void* parameters_ref);

#endif
