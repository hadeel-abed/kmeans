import symnmfmodule as symnmfmod
import numpy as np
import pandas as pd
import sys

np.random.seed(0)

def symnmf(goal, k, vec):
    rows, cols = len(vec), len(vec[0])
    
    def handle_error(func):
        try:
            return func()
        except Exception as e:
            print(f"An Error Has Occurred: {e}")
            sys.exit(1)

    if goal == 'symnmf':
        W = handle_error(lambda: calculate_normalized_similarity_matrix(vec, rows, cols))
        H = handle_error(lambda: initialize_H(W, rows, k))
        result = handle_error(lambda: calculate_H(H, W, rows, k))
        
    elif goal == 'sym':
        result = handle_error(lambda: calculate_similarity_matrix(vec, rows, cols))
        
    elif goal == 'ddg':
        result = handle_error(lambda: calculate_diagonal_degree_matrix(vec, rows, cols))
        
    elif goal == 'norm':
        result = handle_error(lambda: calculate_normalized_similarity_matrix(vec, rows, cols))
        
    else:
        print("Invalid goal specified")
        sys.exit(1)

    return result

def initialize_H(W, rows, k):
    m = np.sum(np.array(W)) / (rows ** 2)  # Compute the mean of W
    return np.random.uniform(0, 2 * np.sqrt(m / k), size=(rows, k)).tolist()

def calculate_similarity_matrix(vec, rows, cols):
    return symnmfmod.sym(vec, rows, cols)

def calculate_diagonal_degree_matrix(vec, rows, cols):
    return symnmfmod.ddg(vec, rows, cols)

def calculate_normalized_similarity_matrix(vec, rows, cols):
    return symnmfmod.norm(vec, rows, cols)

def calculate_H(H, W, rows, k):
    return symnmfmod.symnmf(H, W, rows, k)

def read_file(input_file):
    vec = pd.read_csv(input_file, header=None)
    return vec.values.tolist()

def print_mat(mat):
    for row in mat:
        print(','.join(format(value, ".4f") for value in row))

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: script.py <k> <goal> <input_file>")
        sys.exit(1)

    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]

    vec = read_file(file_name)
    result = symnmf(goal, k, vec)
    print_mat(result)
