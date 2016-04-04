import sys
import random
from sklearn.cross_validation import train_test_split

from cluster import Cluster

DEBUG = False
N_TIMES = 100


def compute_mean_error(clusters):
    error = 0
    for c in clusters:
        error += c.error
    return error / len(clusters)


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
    while loop_counter < 200:
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
            if DEBUG:
                print "Converged after %s iterations" % loop_counter
            break
    return clusters


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

        error += smallest_distance / (abs(clusters[cluster_index].centroid.predict(point)) + 0.0000001)

    return error / len(points)


def fit_functions(points, cutoff=0.1):
    validation_error = []
    train, test = train_test_split(points, train_size=0.8)
    # k = 1 has the same result every time
    clusters = kmeans(train, 1, cutoff)
    result_clusters = clusters
    # error = compute_mean_error(clusters)
    error = compute_mean_error(clusters)

    validation_error.append(compute_test_error(clusters, test))
    print "Error: " + str(validation_error[-1])
    print "Clusters:"
    for c in clusters:
        print "- " + str(c.centroid)

    # A max k must be defined (10 for the moment)
    for k in range(2, 11):
        error = sys.maxint
        for i in range(N_TIMES):
            new_clusters = kmeans(points, k, cutoff)
            new_error = compute_mean_error(new_clusters)
            if new_error < error:
                error = new_error
                clusters = new_clusters
        validation_error.append(compute_test_error(clusters, test))
        print "Error: " + str(validation_error[-1])
        print "Clusters:"
        for c in clusters:
            print "- " + str(c.centroid)
        # break if test_error increases
        if validation_error[-2] < validation_error[-1]:
            break
        result_clusters = clusters
    return validation_error, result_clusters
