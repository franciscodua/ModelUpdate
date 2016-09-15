import sys
import warnings

import util
from KPlane import Interval
from KPlane import kplane
from KPlane.Rule import Rule


def main():
    warnings.filterwarnings("ignore")
    # Generate some points
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    else:
        input_file = raw_input("File containing Samples:")

    points = util.read_points_csv(input_file)

    clusters = kplane.fit_functions(points)

    intervals = []

    for c in clusters:
        print "Regra: "
        print "\tCluster: " + str(c.centroid)
        # prints ranges of values from different dimensions, except output
        ranges = []
        for dim in range(c.n - 1):
            values = [p.coords[dim] for p in c.points]
            # print "\t\tDim " + str(dim) + ": " + "(" + str(min(values)) + ", " + str(max(values)) + ")"
            ranges.append((min(values), max(values)))
        intervals.append(Interval.Interval(ranges))

    resulting_intervals = Interval.split_intervals(intervals)
    for interval in range(len(resulting_intervals)):
        print str(interval) + ": " + str(resulting_intervals[interval].ranges)

    print "Average Accuracy: " + str((1 - kplane.compute_test_error(clusters, points)))

    rules = []
    for interval in resulting_intervals:
        rules.append(Rule(interval, [c.centroid for c in clusters], points))

    print "Done!"
    if len(sys.argv) > 2:
        with open(sys.argv[2], 'w') as f:
            for i in range(len(clusters)):
                for point in clusters[i].points:
                    for x in point.coords:
                        f.write(str(x) + ', ')
                    f.write(str(i) + '\n')


def run_total():
    warnings.filterwarnings("ignore")

    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    else:
        input_file = raw_input("File containing Samples:")

    points = util.read_points_csv(input_file)

    clusters = kplane.fit_functions(points)

    intervals = []

    for c in clusters:
        ranges = []
        for dim in range(c.n - 1):
            values = [p.coords[dim] for p in c.points]
            ranges.append((min(values), max(values)))
        intervals.append(Interval.Interval(ranges))

    resulting_intervals = Interval.split_intervals(intervals)

    rules = []
    for interval in resulting_intervals:
        rules.append(Rule(interval, [c.centroid for c in clusters], points))

    for rule in rules:
        if rule.impact_functions:
            print str(rule.interval.ranges) + ":"
            for impact in rule.impact_functions:
                print "\t" + str(impact.probability) + "\t" + str(impact.function)

    print "Done!"

if __name__ == '__main__':
    main()
