import random
import sys
import warnings

from sklearn import cross_validation

import util
from Visualization import main


def compute_test_error(rules, points):
    error = 0

    for point in points:
        belongs = False
        real = point.coords[-1]
        for rule in rules:
            if not rule.impact_functions:
                continue
            if rule.interval.belongs(point):
                belongs = True
                predictions = []
                for impact in rule.impact_functions:
                    predictions.append(impact.function.predict(point))
                errors = []
                for pred in predictions:
                    if pred == 0:
                        pred += 0.0000001
                    errors.append(abs(real - pred) / abs(pred))
                error += min(errors)
                break
        if not belongs:
            predictions = []
            for rule in rules:
                for impact in rule.impact_functions:
                    predictions.append(impact.function.predict(point))
            errors = []
            for pred in predictions:
                if pred == 0:
                    pred += 0.0000001
                errors.append(abs(real - pred) / abs(pred))
            error += min(errors)

    return error / len(points)


def test_fitting():
    warnings.filterwarnings("ignore")
    if sys.argv > 2:
        # dataset_file = sys.argv[1]
        dataset_file = '/Users/francisco/Documents/IST/Data/logs-complete/clean.csv'
        # output_file = sys.argv[2]
        output_file = '~/out'
    else:
        raise Exception("Error: Not enough arguments")
    dataset = util.read_points_csv(dataset_file)
    dataset_length = len(dataset)

    for size in range(20, int(round(0.8 * dataset_length)), 10):
        kf = cross_validation.KFold(dataset_length, n_folds=10, shuffle=False)
        test_error = []
        for train_index, test_index in kf:
            train = [dataset[i] for i in train_index]
            test = [dataset[i] for i in test_index]
            train_error = []

            for i in range(10):
                current_train = random.sample(train, size)
                rules = main.run_total(current_train)
                train_error.append(compute_test_error(rules, test))

            test_error.append(util.avg(train_error))
        with open(output_file, 'a') as f:
            f.write(str(size) + ', ' + str(util.avg(test_error)) + '\n')
        print('test_error: ' + str(util.avg(test_error)))
