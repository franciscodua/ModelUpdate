import csv
import random
import sys
import warnings

from sklearn import cross_validation

from KPlane import kmeans
from KPlane.objects import Point


# import graph


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


        # for p in train_set:
        #     current_set.append(p)
        #     clusters = kmeans.fit_functions(current_set)
        #     print kmeans.compute_test_error(clusters, test_set)

if __name__ == "__main__":
    # main()
    test_accu()
