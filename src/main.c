/**
 * @file main.c
 * @brief Entry point for the multithreaded matrix pipeline.
 *
 * Orchestrates the full computation: A + B = D, C × D = E, and a global
 * reduction (sum) of E, timing each phase independently.
 *
 * NOTE: This project was recovered from a recorded presentation after the
 * original source files were lost. Comments and documentation are new.
 */

#include <stdio.h>
#include <time.h>
#include "../include/errorMessages.h"
#include "../include/resources.h"
#include "../include/threads.h"

#define file_matrix_A argv[3]
#define file_matrix_B argv[4]
#define file_matrix_C argv[5]
#define file_matrix_D argv[6]
#define file_matrix_E argv[7]

/**
 * @brief Program entry point.
 *
 * Parses and validates command-line arguments, allocates the five matrix
 * structs, and runs the three pipeline stages in order:
 *   1. Sum:          D = A + B
 *   2. Multiply:     E = C × D
 *   3. Reduce:       scalar = sum of all elements in E
 *
 * Each stage is timed individually using clock(). All timing results and the
 * final reduction value are printed to stdout.
 *
 * Usage: ./exec <threads> <dimension> <A> <B> <C> <D> <E>
 *
 * @param argc Argument count; must equal 8.
 * @param argv argv[1] Thread count (accepted values: 1, 2, 4).
 *             argv[2] Matrix dimension N (N×N square matrices).
 *             argv[3] Path to input matrix A.
 *             argv[4] Path to input matrix B.
 *             argv[5] Path to input matrix C.
 *             argv[6] Path to output matrix D.
 *             argv[7] Path to output matrix E.
 * @return 0 on success; exits with EXIT_FAILURE on any error.
 */
int main(int argc, char** argv)
{
    verify_num_args(argc);

    int num_threads = stringToInt(argv[1]);
    verify_num_threads(num_threads);

    int dimension = stringToInt(argv[2]);
    verify_num_dimension(dimension);

    Matrix* matrix_A = newMatrix(file_matrix_A, "r+", dimension);
    Matrix* matrix_B = newMatrix(file_matrix_B, "r+", dimension);
    Matrix* matrix_C = newMatrix(file_matrix_C, "r+", dimension);
    Matrix* matrix_D = newMatrix(file_matrix_D, "w+", dimension);
    Matrix* matrix_E = newMatrix(file_matrix_E, "w+", dimension);

    transcribe_A_and_B(matrix_A, matrix_B, num_threads);

    clock_t start_sum = clock();
    sum(matrix_A, matrix_B, matrix_D, dimension, num_threads);
    clock_t end_sum = clock() - start_sum;

    write_D_transcribe_C(matrix_C, matrix_D, num_threads);

    clock_t start_multiplication = clock();
    multiply(matrix_C, matrix_D, matrix_E, dimension, num_threads);
    clock_t end_multiplication = clock() - start_multiplication;

    clock_t start_reduction = clock();
    long long int reduction = reduce(matrix_E, dimension, num_threads);
    clock_t end_reduction = clock() - start_reduction;

    double total_time_sum = ((double) end_sum) / CLOCKS_PER_SEC;
    double total_time_multiplication = ((double) end_multiplication) / CLOCKS_PER_SEC;
    double total_time_reduction = ((double) end_reduction) / CLOCKS_PER_SEC;
    double total_time_global = total_time_sum + total_time_multiplication + total_time_reduction;

    printf("Reduction: %lld\n", reduction);
    printf("Time Sum: %lf\n", total_time_sum);
    printf("Time Multiplication: %lf\n", total_time_multiplication);
    printf("Time Reduction: %lf\n", total_time_reduction);
    printf("Time Global: %lf\n", total_time_global);

    return 0;
}
