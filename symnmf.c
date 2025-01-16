#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "symnmf.h"

double **allocate_matrix(double **matrix, int rows, int cols)
{
    int index;
    matrix = malloc(rows * sizeof(double *));
    if (matrix == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    for (index = 0; index < rows; index++)
    {
        matrix[index] = (double *)calloc(cols, sizeof(double));
        if (matrix[index] == NULL)
        {
            printf("Memory allocation failed for row %d\n", index);
            return NULL;
        }
    }
    return matrix;
}

double **create_similarity_matrix(double **data, int rows, int cols)
{
    double **similarity_matrix = NULL;
    int row, col;
    double value, dist;

    similarity_matrix = allocate_matrix(similarity_matrix, rows, rows);
    if (similarity_matrix == NULL)
    {
        return NULL;
    }

    for (row = 0; row < rows; row++)
    {
        for (col = row; col < rows; col++)
        {
            if (row == col)
            {
                value = 0;
            }
            else
            {
                dist = ((-1) * (calculate_euclidean_distance(data[row], data[col], cols)) / 2);
                value = exp(dist);
            }
            similarity_matrix[row][col] = value;
            similarity_matrix[col][row] = value;
        }
    }
    return similarity_matrix;
}

double **compute_diagonal_matrix(double **matrix, int size)
{
    double **result = NULL;
    int row, col;
    double sum;

    result = allocate_matrix(result, size, size);
    if (result == NULL)
    {
        return NULL;
    }

    for (row = 0; row < size; row++)
    {
        sum = 0;
        for (col = 0; col < size; col++)
        {
            sum += matrix[row][col];
        }
        result[row][row] = sum;
    }

    return result;
}

double **multiply_matrices(double **matrix1, double **matrix2, int rows1, int common_dim, int cols2)
{ 
    double **result = NULL;
    int row, col, index;
    
    result = allocate_matrix(result, rows1, cols2);
    if (result == NULL)
    {
        return NULL;
    }

    for (row = 0; row < rows1; row++)
    {
        for (col = 0; col < cols2; col++)
        {
            result[row][col] = 0;
            for (index = 0; index < common_dim; index++)
            {
                result[row][col] += matrix1[row][index] * matrix2[index][col];
            }
        }
    }
    return result;
}

double calculate_frobenius_norm(double **matrix, int num_rows, int num_cols)
{ 
    double result = 0;
    int row_idx, col_idx;
    for (row_idx = 0; row_idx < num_rows; row_idx++)
    {
        for (col_idx = 0; col_idx < num_cols; col_idx++)
        {
            result += (matrix[row_idx][col_idx]) * (matrix[row_idx][col_idx]);
        }
    }
    return sqrt(result);
}

double **transpose_matrix(double **matrix, int num_rows, int num_cols)
{ 
    int r_idx, j_idx;
    double **result = NULL;
    result = allocate_matrix(result, num_cols, num_rows);
    if (result == NULL)
    {
        return NULL;
    }

    for (r_idx = 0; r_idx < num_cols; r_idx++)
    {
        for (j_idx = 0; j_idx < num_rows; j_idx++)
        {
            result[r_idx][j_idx] = matrix[j_idx][r_idx];
        }
    }
    return result;
}

double **optimize_H_matrix(double **initial_H, double **W_matrix, int num_rows, int num_cols, double epsilon_threshold, int max_iterations)
{
    double **previous_H_matrix, **updated_H_matrix = NULL, **WH_product, **HHt_product, **Ht_transpose, **HHtH_product;
    double norm_difference;
    int iteration = 0, row_idx, col_idx;
    epsilon_threshold = 1e-4;
    max_iterations = 300;
    previous_H_matrix = initial_H;

    while (iteration < max_iterations)
    {
        iteration++;
        updated_H_matrix = allocate_matrix(updated_H_matrix, num_rows, num_cols);
        if (updated_H_matrix == NULL)
        {
            return NULL;
        }
        Ht_transpose = transpose_matrix(previous_H_matrix, num_rows, num_cols);
        WH_product = multiply_matrices(W_matrix, previous_H_matrix, num_rows, num_rows, num_cols);
        HHt_product = multiply_matrices(previous_H_matrix, Ht_transpose, num_rows, num_cols, num_rows);
        HHtH_product = multiply_matrices(HHt_product, previous_H_matrix, num_rows, num_rows, num_cols);
        
        for (row_idx = 0; row_idx < num_rows; row_idx++)
        {
            for (col_idx = 0; col_idx < num_cols; col_idx++)
            {
                updated_H_matrix[row_idx][col_idx] = previous_H_matrix[row_idx][col_idx] * 
                    (0.5 + (0.5 * (WH_product[row_idx][col_idx] / HHtH_product[row_idx][col_idx])));
            }
        }
        for (row_idx = 0; row_idx < num_rows; row_idx++)
        {
            for (col_idx = 0; col_idx < num_cols; col_idx++)
            {
                previous_H_matrix[row_idx][col_idx] = updated_H_matrix[row_idx][col_idx] - previous_H_matrix[row_idx][col_idx];
            }
        }

        norm_difference = calculate_frobenius_norm(previous_H_matrix, num_rows, num_cols);
        release_matrix(Ht_transpose, num_cols);
        release_matrix(HHt_product, num_rows);
        release_matrix(WH_product, num_rows);
        release_matrix(HHtH_product, num_rows);

        if (norm_difference < epsilon_threshold)
        {
            return updated_H_matrix;
        }
        release_matrix(previous_H_matrix, num_rows);
        previous_H_matrix = updated_H_matrix;
    }

    return updated_H_matrix;
}

double **deal_with_input(char *input_file, int *rows, int *cols)
{
    FILE *file = NULL;
    char ch;
    int i, j;
    double **result = NULL;
    double cur;
    int size = 0;
    
    file = fopen(input_file, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", input_file);
        return NULL;
    }

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            (*rows)++;
            size++;
        } else if (ch == ',') {
            size++;
        }
    }

    rewind(file);
    *cols = size / *rows;

    result = allocate_matrix(NULL, *rows, *cols);
    if (result == NULL) {
        fclose(file);
        return NULL;
    }

    for (i = 0; i < *rows; i++) {
        for (j = 0; j < *cols; j++) {
            if (fscanf(file, "%lf", &cur) != 1) {
                fprintf(stderr, "Error: Failed to read double value\n");
                release_matrix(result, *rows);
                fclose(file);
                return NULL;
            }
            result[i][j] = cur;
            fgetc(file);
        }
    }

    fclose(file);
    return result;
}

double **sym(double **X, int n, int d)
{
    return create_similarity_matrix(X, n, d);
}

double **ddg(double **X, int n, int d)
{
    double **D;
    double **A = create_similarity_matrix(X, n, d);
    if (A == NULL)
    {
        return NULL;
    }
    D = compute_diagonal_matrix(A, n);
    release_matrix(A, n);
    return D;
}

double **compute_normalized_matrix(double **X, int n, int d) {
    int index;
    double **diagonal;
    double **normalized;
    double **temp;
    double **A = create_similarity_matrix(X, n, d);

    if (A == NULL) {
        return NULL;
    }

    diagonal = compute_diagonal_matrix(A, n);
    if (diagonal == NULL) {
        release_matrix(A, n);
        return NULL;
    }

    for (index = 0; index < n; index++) {
        if (diagonal[index][index] == 0) {
           printf("divide by zero,");
        } else {
            diagonal[index][index] = pow(diagonal[index][index], -0.5);
        }
    }

    normalized = multiply_matrices(diagonal, A, n, n, n);
    if (normalized == NULL) {
        release_matrix(A, n);
        release_matrix(diagonal, n);
        return NULL;
    }

    temp = normalized;
    normalized = multiply_matrices(normalized, diagonal, n, n, n);
    if (normalized == NULL) {
        release_matrix(temp, n);
        release_matrix(A, n);
        release_matrix(diagonal, n);
        return NULL;
    } else {
        release_matrix(temp, n);
        release_matrix(A, n);
        release_matrix(diagonal, n);
    }
    return normalized;
}

void myPrint(double **mat, int n, int d)
{ 
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < d; j++)
        {
            printf("%.4f", mat[i][j]);
            if (j < d - 1)
            {
                printf(",");
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    double **input;
    char *function_name, *file_path;
    double **result = NULL;
    int n = 0, d = 0;
    
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <function_name> <file_path>\n", argv[0]);
        return 1;
    }

    function_name = argv[1];
    file_path = argv[2];

    input = deal_with_input(file_path, &n, &d);
    if (input == NULL)
    {
        fprintf(stderr, "Error reading input from file: %s\n", file_path);
        return 1;
    }

    if (strcmp(function_name, "sym") == 0)
    {
        result = sym(input, n, d);
    }
    else if (strcmp(function_name, "ddg") == 0)
    {
        result = ddg(input, n, d);
    }
    else if (strcmp(function_name, "norm") == 0)
    {
        result = compute_normalized_matrix(input, n, d);
    }
    else
    {
        fprintf(stderr, "Unknown function: %s\n", function_name);
        release_matrix(input, n);
        return 1;
    }

    if (result == NULL)
    {
        fprintf(stderr, "Error computing result.\n");
        release_matrix(input, n);
        return 1;
    }

    myPrint(result, n, n);
    release_matrix(result, n);
    release_matrix(input, n);

    return 0;
}

double calculate_euclidean_distance(double *array1, double *array2, int length)
{ 
    double distance_squared = 0;
    int index = 0;

    while (index < length)
    {
        distance_squared += (array1[index] - array2[index]) * (array1[index] - array2[index]);
        index++;
    }
    return distance_squared;
}

void release_matrix(double **matrix, int rows)
{ 
    int row_index = 0;
    while (row_index < rows)
    {
        if (matrix[row_index] != NULL) {
            free(matrix[row_index]);
        }
        row_index++;
    }
    free(matrix);
}