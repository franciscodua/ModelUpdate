import random

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
    while loop_counter < 1000:
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


def fit_functions(points, cutoff=0.1):
    error_values = []
    # k = 1 has the same result every time
    clusters = kmeans(points, 1, cutoff)
    # print "Now Clustering for K = 1"
    # error = compute_mean_error(clusters)
    error = compute_mean_error(clusters)
    # save previous state
    previous_error = error
    previous_clusters = clusters

    error_values.append(error)
    # A max k must be defined (10 for the moment)
    for k in range(2, 31):
        # print "Now Clustering for K = %s" % k
        for i in range(N_TIMES):
            new_clusters = kmeans(points, k, cutoff)
            # new_error = compute_mean_error(new_clusters)
            new_error = compute_mean_error(new_clusters)
            if new_error < error:
                error = new_error
                clusters = new_clusters

        error_values.append(error)
        # if error begins to stagnate stop
        if previous_error - error < 0.04:
            clusters = previous_clusters
            break

        previous_clusters = clusters
        previous_error = error

    return error_values, clusters
