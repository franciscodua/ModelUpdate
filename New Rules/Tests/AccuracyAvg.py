import random
import sys
import warnings

from sklearn import cross_validation

import util
from KPlane import Interval
from KPlane import kplane
from KPlane.Rule import Rule


def find_rules(dataset):
    clusters = kplane.fit_functions(dataset)

    intervals = [Interval.Interval([(1, 4), (0, 590), (0, 45)])]

    # for c in clusters:
    #     ranges = []
    #     for dim in range(c.n - 1):
    #         values = [p.coords[dim] for p in c.points]
    #         # print "\t\tDim " + str(dim) + ": " + "(" + str(min(values)) + ", " + str(max(values)) + ")"
    #         ranges.append((round(min(values)), round(max(values))))
    #     intervals.append(Interval.Interval(ranges))

    resulting_intervals = Interval.split_intervals(intervals)

    rules = []
    for interval in resulting_intervals:
        new_rule = Rule(interval, [c.centroid for c in clusters], dataset)
        if len(new_rule.impact_functions) > 0:
            rules.append(new_rule)

    return rules


def compute_test_error(rules, points):
    error = 0

    for point in points:
        for rule in rules:
            if rule.interval.belongs(point):
                prediction = rule.predict(point)
                real = point.coords[-1]
                if prediction == 0:
                    prediction += 0.0000001
                error += abs(real - prediction) / abs(prediction)
                break

    return error / len(points)


def test():
    warnings.filterwarnings("ignore")

    dataset_file = sys.argv[1]
    output_file = sys.argv[2]

    dataset = util.read_points_csv(dataset_file)

    find_rules(dataset)

    for size in range(20, 25, 5):
        kf = cross_validation.KFold(len(dataset), n_folds=10, shuffle=False)
        test_error = []
        for train_index, test_index in kf:
            train = [dataset[i] for i in train_index]
            test = [dataset[i] for i in test_index]
            train_error = []

            for i in range(10):
                current_train = random.sample(train, size)
                rules = find_rules(current_train)
                train_error.append(compute_test_error(rules, test))

            test_error.append(util.avg(train_error))
        with open(output_file, 'a') as f:
            f.write(str(size) + ', ' + str(util.avg(test_error)) + '\n')
        print('test_error: ' + str(util.avg(test_error)))
