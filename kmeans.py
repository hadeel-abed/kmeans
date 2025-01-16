import math
import sys
import random

def euclidean_distance(p1, p2):
    return math.sqrt(sum((a - b) ** 2 for a, b in zip(p1, p2)))

def k_means(data, k, max_iter, epsilon):
    centroids = random.sample(data, k)
    
    for _ in range(max_iter):
        clusters = [[] for _ in range(k)]
        
        for point in data:
            closest_centroid = min(range(k), key=lambda i: euclidean_distance(point, centroids[i]))
            clusters[closest_centroid].append(point)
        
        new_centroids = []
        for cluster in clusters:
            if cluster:
                new_centroid = [sum(dim) / len(cluster) for dim in zip(*cluster)]
                new_centroids.append(new_centroid)
            else:
                new_centroids.append(random.choice(data))
        
        if all(euclidean_distance(c1, c2) < epsilon for c1, c2 in zip(centroids, new_centroids)):
            break
        
        centroids = new_centroids
    
    return centroids

def read_data_from_file(filename):
    with open(filename, 'r') as f:
        return [list(map(float, line.strip().split(','))) for line in f]

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("An Error Has Occurred")
        sys.exit(1)

    k = int(sys.argv[1])
    max_iter = 200
    filename = sys.argv[2]

    try:
        data = read_data_from_file(filename)
        centroids = k_means(data, k, max_iter, epsilon=0.001)
        
        for centroid in centroids:
            print(','.join(f'{coord:.4f}' for coord in centroid))
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)