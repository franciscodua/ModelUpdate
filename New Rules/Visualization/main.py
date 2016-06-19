import sys
import warnings

import util
from KPlane import kplane


def main():
    warnings.filterwarnings("ignore")
    # Generate some points
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    else:
        input_file = raw_input("File containing Samples:")

    points = util.read_points_csv(input_file)

    clusters = kplane.fit_functions(points)

    for c in clusters:
        print "Regra: "
        print "\tCluster: " + str(c.centroid)
        # prints ranges of values from different dimensions, except output
        for dim in range(c.n - 1):
            values = [p.coords[dim] for p in c.points]
            print "\t\tDim " + str(dim) + ": " + "(" + str(min(values)) + ", " + str(max(values)) + ")"

    print "Average Accuracy: " + str((1 - kplane.compute_test_error(clusters, points)))
