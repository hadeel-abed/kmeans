#ifndef SYMNMF_H
#define SYMNMF_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/** Allocates memory for a matrix of size rows x cols */
double **allocate_matrix(double **matrix, int rows, int cols);

/** Calculates the Euclidean distance between two vectors */
double calculate_euclidean_distance(double *array1, double *array2, int length);

/** Forms a similarity matrix from the input data */
double **create_similarity_matrix(double **data, int rows, int cols);

/** Calculates the diagonal degree matrix from the similarity matrix */
double **compute_diagonal_matrix(double **similarity_matrix, int size);

/** Multiplies two matrices */
double **multiply_matrices(double **matrix1, double **matrix2, int rows1, int common_dim, int cols2);

/** Computes the normalized similarity matrix W */
double **compute_normalized_matrix(double **X, int n, int d);

/** Calculates the Frobenius norm of a matrix */
double calculate_frobenius_norm(double **matrix, int num_rows, int num_cols);

/** Returns the transpose of a matrix */
double **calculate_transpose_matrix(double **mat, int n, int d);

/** Updates matrix H iteratively */
double **optimize_H_matrix(double **initial_H, double **W_matrix, int num_rows, int num_cols, double epsilon_threshold, int max_iterations);

/** Reads a matrix from a file */
double **deal_with_input(char *input_file, int *rows, int *cols);

/** Computes the similarity matrix */
double **sym(double **X, int n, int d);

/** Computes the diagonal degree matrix */
double **ddg(double **X, int n, int d);

/** Computes the normalized matrix */
double **compute_normalized_matrix(double **X, int n, int d);

/** Prints a matrix */
void myPrint(double **mat, int n, int d);

/** Frees the memory allocated for a matrix */
void release_matrix(double **matrix, int rows);


#endif
