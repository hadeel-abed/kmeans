# Importing required modules
from symnmf import symnmf
from kmeans import k_means, read_data_from_file, distance
from sklearn.metrics import silhouette_score
import sys



def main():
    # Default parameters
    epsilon = 0.0001
    default_iterations = 200
    max_iterations = 300

    # Parse command-line arguments
    if len(sys.argv) not in [3, 4]:
        print("Usage: python script.py <k> <file_name> [<iterations>]")
        sys.exit(1)

    k = int(sys.argv[1])
    file_name = sys.argv[2]
    iterations = int(sys.argv[3]) if len(sys.argv) == 4 else default_iterations

    # Read data from file
    try:
        X = read_data_from_file(file_name)
    except Exception as e:
        print(f"Error reading data from file: {e}")
        sys.exit(1)

    # Perform SymNMF
    try:
        H = symnmf('symnmf', k, X)
    except Exception as e:
        print(f"Error performing SymNMF: {e}")
        sys.exit(1)

    # Perform K-means clustering
    try:
        centroids = k_means(X, k, iterations, epsilon)
    except Exception as e:
        print(f"Error performing K-means clustering: {e}")
        sys.exit(1)

    # Assign clusters for SymNMF
    assign_sym = [
        max(range(len(H[0])), key=lambda x: H[i][x])
        for i in range(len(H))
    ]

    # Assign clusters for K-means
    assign_kmeans = [
        min(range(len(centroids)), key=lambda k: distance(X[i], centroids[k]))
        for i in range(len(X))
    ]

    # Calculate and print silhouette scores
    try:
        silhouette_nmf = silhouette_score(X, assign_sym)
        silhouette_kmeans = silhouette_score(X, assign_kmeans)
        print(f"nmf: {silhouette_nmf:.4f}")
        print(f"kmeans: {silhouette_kmeans:.4f}")
    except ValueError as e:
        print(f"Error calculating silhouette score: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
