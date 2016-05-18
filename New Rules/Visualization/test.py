import csv
import sys
import warnings

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
    train_file = sys.argv[1]
    test_file = sys.argv[2]

    train_set = read_points_csv(train_file)
    test_set = read_points_csv(test_file)

    current_set = train_set[:9]
    train_set = train_set[9:]

    for p in train_set:
        current_set.append(p)
        clusters = kmeans.fit_functions(current_set)
        print kmeans.compute_test_error(clusters, test_set)

if __name__ == "__main__":
    # main()
    test_accu()
