import sys
import csv
import warnings

from objects import Point
import kmeans
import graph


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
        output_graph = "test.png"

    points = read_points_csv(input_file)

    error_values, clusters = kmeans.fit_functions(points)

    for c in clusters:
        print "Cluster: " + str(c.centroid) + ". Error: " + str(c.error)

    #graph.new_graph(clusters, output_graph)
    graph.graph_cost(error_values, output_graph)


if __name__ == "__main__":
    main()
