import sys
import csv

from objects import Point
import kmeans


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
    # Generate some points
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    else:
        input_file = raw_input("File containing Samples:")

    points = read_points_csv(input_file)

    # When do we say the optimization has 'converged' and stop updating clusters
    opt_cutoff = 0.5

    clusters = kmeans.fit_functions(points, opt_cutoff)

    for c in clusters:
        print "Cluster: " + str(c.centroid) + ". Accuracy: " + str(c.error)


if __name__ == "__main__":
    main()
