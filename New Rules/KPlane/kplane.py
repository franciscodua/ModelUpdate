import random
import sys

from sklearn import cross_validation

import util
from cluster import Cluster

DEBUG = False
N_TIMES = 10
ERROR_THRES = 0.05


def compute_mean_error(clusters):
    error = 0
    for c in clusters:
        error += c.error
    return error / len(clusters)


def compute_test_error(clusters, points):
    cluster_count = len(clusters)
    error = 0

    for point in points:
        smallest_distance = clusters[0].centroid.get_error(point)
        cluster_index = 0
        for i in range(1, cluster_count):
            distance = clusters[i].centroid.get_error(point)
            if distance < smallest_distance:
                smallest_distance = distance
                cluster_index = i

        error += smallest_distance / (abs(clusters[cluster_index].centroid.predict(point)))

    return error / len(points)


def kmeans(points, k, cutoff):
    """
    kmeans variant: use functions as clusters centres.
    Similar to K-Plane
    :param points: list of points
    :param k: number of clusters
    :param cutoff: shift distance below which it should stop
    :return: list of clusters found
    """

    dimension = points[0].n
    # Create k clusters using those centroids
    clusters = [Cluster(random.sample(points, dimension)) for i in range(k)]

    # Loop through the dataset until the clusters stabilize
    loop_counter = 0
    while loop_counter < 100:
        # Create a list of lists to hold the points in each cluster
        lists = [[] for c in clusters]
        cluster_count = len(clusters)

        # Start counting loops
        loop_counter += 1
        # For every point in the dataset ...
        for p in points:
            # Get the distance between that point and the centroid of the first
            # cluster.
            smallest_distance = clusters[0].centroid.get_error(p)

            # Set the cluster this point belongs to
            cluster_index = 0

            # For the remainder of the clusters ...
            for i in range(cluster_count - 1):
                # calculate the distance of that point to each other cluster's
                # centroid.
                distance = clusters[i + 1].centroid.get_error(p)
                # If it's closer to that cluster's centroid update what we
                # think the smallest distance is, and set the point to belong
                # to that cluster
                if distance < smallest_distance:
                    smallest_distance = distance
                    cluster_index = i + 1
            lists[cluster_index].append(p)

        for lst in lists:
            if len(lst) < dimension:
                lst.extend(random.sample(points, dimension))

        # Set our biggest_shift to zero for this iteration
        biggest_shift = 0.0

        # As many times as there are clusters ...
        for i in range(cluster_count):
            # Calculate how far the centroid moved in this iteration
            shift = clusters[i].update(lists[i])
            # Keep track of the largest move from all cluster centroid updates
            biggest_shift = max(biggest_shift, shift)

        # If the centroids have stopped moving much, say we're done!
        if biggest_shift < cutoff:
            break
    return clusters


def choose_k(points, cutoff=0.1):
    kf = cross_validation.KFold(len(points), n_folds=10, shuffle=True)
    validation_error = []
    current_error = []
    optimal_k = 1
    clusters = []
    # for k = 1
    for train_index, test_index in kf:
        train = [points[i] for i in train_index]
        test = [points[i] for i in test_index]
        clusters = kmeans(train, 1, cutoff)
        current_error.append(compute_test_error(clusters, test))
    validation_error.append(util.avg(current_error))
    if validation_error[-1] < ERROR_THRES:
        return 1

    for k in range(2, 31):
        current_error = []
        error = sys.maxint
        for train_index, test_index in kf:
            train = [points[i] for i in train_index]
            test = [points[i] for i in test_index]
            for i in range(N_TIMES):
                new_clusters = kmeans(train, k, cutoff)
                new_error = compute_mean_error(new_clusters)
                if new_error < error:
                    error = new_error
                    clusters = new_clusters

            current_error.append(compute_test_error(clusters, test))

        validation_error.append(util.avg(current_error))

        optimal_k = k
        if validation_error[-1] < ERROR_THRES:
            break

    return optimal_k


def fit_functions(points, cutoff=0.1):
    k = choose_k(points, cutoff)
    error = sys.maxint
    clusters = []
    for i in range(2 * N_TIMES):
        new_clusters = kmeans(points, k, cutoff)
        new_error = compute_mean_error(new_clusters)
        if new_error < error:
            error = new_error
            clusters = new_clusters
    return clusters
