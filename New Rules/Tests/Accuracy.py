import random
import sys
import warnings

from sklearn import cross_validation

import util
from KPlane import kplane
from Visualization import main


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


def test_acc_speed():
    """
    Compute average accuracy for different number of initial samples with different sets of samples
    Assumes the existence of a initial dataset (generated)
    """
    warnings.filterwarnings("ignore")
    if sys.argv > 3:
        generated_file = sys.argv[1]
        dataset_file = sys.argv[2]
        output_file = sys.argv[3]
    else:
        raise Exception("Error: Not enough arguments")

    generated = util.read_points_csv(generated_file)
    dataset = util.read_points_csv(dataset_file)

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
                clusters = kplane.fit_functions(current_train)
                train_error.append(kplane.compute_test_error(clusters, test))

            test_error.append(util.avg(train_error))
        with open(output_file, 'a') as f:
            f.write(str(size) + ', ' + str(util.avg(test_error)) + '\n')
        print('test_error: ' + str(util.avg(test_error)))


def test_fitting():
    warnings.filterwarnings("ignore")
    if sys.argv > 2:
        dataset_file = sys.argv[1]
        output_file = sys.argv[2]
    else:
        raise Exception("Error: Not enough arguments")
    dataset = util.read_points_csv(dataset_file)
    dataset_length = len(dataset)

    for size in range(20, 0.8 * dataset_length, 10):
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
