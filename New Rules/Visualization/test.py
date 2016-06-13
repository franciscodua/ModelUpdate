import csv
import random
import sys
import warnings
from sklearn import cross_validation

from KPlane import kmeans
from KPlane.objects import Point

# import graph
from KPlane.objects import Function


def read_points_csv(input_file):
    """
    Reads points from CSV file and creates Point for each line
    :param input_file: string with file name
    :return: list of points
    """
    f = open(input_file)
    points = []
    with f as csv_file:
        reader = csv.reader(csv_file, delimiter=',')
        for row in reader:
            points.append(Point([float(x) for x in row]))
    f.close()

    return points


def main():
    warnings.filterwarnings("ignore")
    # Generate some points
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    else:
        input_file = raw_input("File containing Samples:")

    if len(sys.argv) > 2:
        output_graph = sys.argv[2]
    else:
        output_graph = "test"

    points = read_points_csv(input_file)

    clusters = kmeans.fit_functions(points)

    for c in clusters:
        print "Regra: "
        print "\tCluster: " + str(c.centroid)
        # prints ranges of values from different dimensions, except output
        for dim in range(c.n - 1):
            values = [p.coords[dim] for p in c.points]
            print "\t\tDim " + str(dim) + ": " + "(" + str(min(values)) + ", " + str(max(values)) + ")"

    print "Average Accuracy: " + str((1 - kmeans.compute_mean_error(clusters)))
    # graph.new_graph(clusters, output_graph)
    # graph.graph_cost(error_values, output_graph)


def test_accu():
    warnings.filterwarnings("ignore")
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    dataset = read_points_csv(input_file)

    for size in range(20, int(len(dataset) * 0.9), 5):

        kf = cross_validation.KFold(len(dataset), n_folds=10, shuffle=False)
        test_error = []
        for train_index, test_index in kf:
            train = [dataset[i] for i in train_index]
            test = [dataset[i] for i in test_index]
            train_error = []

            for i in range(5):
                clusters = kmeans.fit_functions(random.sample(train, size))
                train_error.append(kmeans.compute_test_error(clusters, test))

            test_error.append(kmeans.avg(train_error))
        with open(output_file, 'a') as f:
            f.write(str(size) + ', ' + str(kmeans.avg(test_error)) + '\n')
        print(str(kmeans.avg(test_error)))


def get_index(functions, points):
    cluster_count = len(functions)

    indexes = [0 for i in range(cluster_count)]

    for point in points:
        smallest_distance = functions[0].get_error(point)
        cluster_index = 0
        for i in range(1, cluster_count):
            distance = functions[i].get_error(point)
            if distance < smallest_distance:
                smallest_distance = distance
                cluster_index = i

        indexes[cluster_index] += 1
    return indexes


def valid(point):
    coords = point.coords
    return 522 >= coords[1] >= 116 and 0.022 >= coords[2] >= 0.008


def prob():
    input_file = sys.argv[1]
    dataset = read_points_csv(input_file)
    points = []
    for point in dataset:
        if valid(point):
            points.append(point)

    total = len(points)

    functions = [Function([0.006756189912123364, 0.00010626211026767887, -0.26020039657094646, -0.020387988431537735]),
                 Function([3.996995856578195e-05, 1.5579816856309757e-05, 0.17784442446838544, 0.0048137458924997064]),
                 Function([-5.496624477064749e-05, -7.587316567235051e-06, 1.377645265505832, -0.0019374432269883765])]

    i = get_index(functions, points)

    print "indexes = " + str(i)

    print "total = " + str(total)

    print "probs = " + str([x / total for x in i])


def speed():
    warnings.filterwarnings("ignore")

    train_file = sys.argv[1]
    test_file = sys.argv[2]
    output_file = sys.argv[3]
    train_points = read_points_csv(train_file)
    test_points = read_points_csv(test_file)

    l = len(test_points) / 2

    for i in range(0, len(test_points) / 2, 5):
        train_points.extend(test_points[0:i])
        test_points = test_points[i:]
        clusters = kmeans.fit_functions(train_points)
        test_error = kmeans.compute_test_error(clusters, test_points)

        with open(output_file, 'a') as f:
            f.write(str(i) + ', ' + str(test_error) + '\n')

def test_acc_speed():
    warnings.filterwarnings("ignore")
    generated_file = sys.argv[1]
    dataset_file = sys.argv[2]
    output_file = sys.argv[3]

    generated = read_points_csv(generated_file)
    dataset = read_points_csv(dataset_file)

    for size in range(0, 305, 5):
        kf = cross_validation.KFold(len(dataset), n_folds=10, shuffle=False)
        test_error = []
        for train_index, test_index in kf:
            train = [dataset[i] for i in train_index]
            test = [dataset[i] for i in test_index]
            train_error = []

            for i in range(10):
                current_train = random.sample(train, size)
                current_train.extend(generated)
                clusters = kmeans.fit_functions(current_train)
                train_error.append(kmeans.compute_test_error(clusters, test))

            test_error.append(kmeans.avg(train_error))
        with open(output_file, 'a') as f:
            f.write(str(size) + ', ' + str(kmeans.avg(test_error)) + '\n')
        print('test_error: ' + str(kmeans.avg(test_error)))


if __name__ == "__main__":
    # main()
    test_accu()
