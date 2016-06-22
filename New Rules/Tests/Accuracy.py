import random
import sys
import warnings

from sklearn import cross_validation

import util
from KPlane import kplane


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
